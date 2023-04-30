#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<locale.h>

#include<pthread.h>
#include<curl/curl.h>

#include"rwcfg.h"
#include"argparse.h"
#include"utils.h"
#include"ezylog.h"
#include"openai_api.h"
#include"cli.h"
#include"crich.h"

void get_remote_version();
bool is_local_latest();

int main( int argc , char** argv ){
    setlocale( LC_ALL , "" );

    usrhome = getenv( "HOME" );
    // get usr home absolute path
    cfgdir = ( char* ) malloc( strlen( usrhome ) + strlen( "/.cgpterm" ) + 1 );
    sprintf( cfgdir , "%s%s" , usrhome , "/.cgpterm" );
    struct stat st = { 0 };
    if ( stat( cfgdir , &st ) == -1 )
    {
        if ( mkdir( cfgdir , 0700 ) == -1 )
        {
            perror( "Make Config DIR failed" );
            return 1;
        } // try make cfgdir; when failed, stop
    } // cfgdir doesn't exist
    // make cfgdir absolute path str, check if cfgdir exists; if not, make it

    cfginipath = ( char* ) malloc( strlen( cfgdir ) + strlen( "/config.ini" ) + 1 );
    sprintf( cfginipath , "%s%s" , cfgdir , "/config.ini" );
    if ( access( cfginipath , F_OK ) == -1 )
    {
        if ( create_config( cfginipath ) == -1 )
        {
            return 1;
        } // try make config.ini; when failed, stop
    } // config.ini not exists
    rconfig( cfginipath );
    // make config.ini absolute path str, check if config.ini exists; if not, make it and dump into a default config data

    logpath = ( char* ) malloc( strlen( cfgdir ) + strlen( "/chat.log" ) + 1 );
    sprintf( logpath , "%s%s" , cfgdir , "/chat.log" );
    logger = ezylog_init( "chat" , "[%T] %l: %P  %m" , logpath , EZYLOG_LOGMODE_APPEND , LOG_LEVEL );
    // make log file path; init logger and launch

    ezylog_logdebug( logger , "cGPTerm master process initialization complete, logger launch" );

    // ============================================================================================
    //                              End of cGPTerm master process init
    // ============================================================================================

    args_init();
    int args_parse_rc = args_parse( argc , argv );
    if ( args_parse_rc != 0 )
        goto stopmain;
    // illegal args
    
    if ( gota_help )
    {
        print_help();
        ezylog_logdebug( logger , "help message printed" );
        goto stopmain;
    }
    int action_setcfg_num = args_actions_setcfg();
    if ( action_setcfg_num == -1 )
    {
        fprintf( stderr , "[main] -> Failed to write new configs, stop\n" );
        ezylog_logfatal( logger , "write new config failed, stop" );
        goto stopmain;
    } // write new config failed
    if ( action_setcfg_num > 0 )
        goto stopmain;
        // wrote new config, stop
    // parse args; show help if triggered; when new config set, goto stopmain

    // ============================================================================================
    //                                      Finish args parse
    // ============================================================================================

    // from here: cGPTerm main service

    curl_global_init( CURL_GLOBAL_ALL );
    curl_version_info_data *curl_version_data = curl_version_info( CURLVERSION_NOW );
    curl_userangel_data = ( char* ) malloc( 16 );
    sprintf( curl_userangel_data , "curl/%s" , curl_version_data -> version );
    // curl init

    pthread_mutex_init( &remote_version_mutex , NULL );
    // init access remote version mutex

    pthread_t get_remote_version_thread;
    pthread_attr_t get_remote_version_thread_attr;
    pthread_attr_init( &get_remote_version_thread_attr );
    pthread_attr_setdetachstate( &get_remote_version_thread_attr , PTHREAD_CREATE_DETACHED );
    ezylog_logdebug( logger , "start getting remote version in background" );
    pthread_create( &get_remote_version_thread , &get_remote_version_thread_attr , get_remote_version , NULL );
    pthread_attr_destroy( &get_remote_version_thread_attr );
    // get remote version in background

    ezylog_logdebug( logger , "config set not triggered, start initializing openai service" );
    openai_init();
    ezylog_logdebug( logger , "openai service initialization complete" );

    crprint( "[dim]Hi, welcome to chat with GPT. Type `[bright magenta]/help[/]` to display available commands.\n" );
    ezylog_loginfo( logger , "cGPTerm main service launch, local version: %s" , CGPTERM_VERSION );

    if ( gota_load )
        openai_load_history( gav_load );

    int CLI_returncode = start_CLI();
    
    printf( "Exiting...\n" );
    crprint( "[bright magenta]Total tokens spent: %ld\n" , openai -> total_tokens_spent );
    // printf( "Total tokens spent: %ld\n" , openai -> total_tokens_spent );
    ezylog_loginfo( logger , "Total tokens spent: %d" , openai -> total_tokens_spent );
    openai_free();
    curl_global_cleanup();

    pthread_mutex_lock( &remote_version_mutex );
    if ( !is_local_latest() )
    {
        crprint( "New Version Available: [bold][red]%s[/][/] -> [bold][green]%s[/][/]\n" , CGPTERM_VERSION , remote_version );
        crprint( "Visit the GitHub Site [underline][bold][blue]https://github.com/Ace-Radom/cGPTerm[/][/][/] to see what have been changed!\n" );
    } // local not latest
    pthread_mutex_unlock( &remote_version_mutex );

stopmain:
    cconfig();
    ezylog_loginfo( logger , "cGPTerm master process shutting down..." );
    ezylog_close( logger );
    return 0;
} // main

void get_remote_version(){
    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when getting remote version" );
        pthread_mutex_lock( &remote_version_mutex );
        remote_version = "Unknown";
        pthread_mutex_unlock( &remote_version_mutex );
        return;
    } // curl init error, set remote_version to "Unknown"

    curl_easy_setopt( curl , CURLOPT_URL , "https://api.github.com/repos/Ace-Radom/cGPTerm/releases" );
    curl_easy_setopt( curl , CURLOPT_HTTPGET , 1L );
    curl_easy_setopt( curl , CURLOPT_USERAGENT , curl_userangel_data );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT , 10 );
    // remote version ask's timeout is set to 10s
    // make curl request

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        ezylog_logerror( logger , "send request failed when getting remote version: " , curl_easy_strerror( res ) );
        pthread_mutex_lock( &remote_version_mutex );
        remote_version = "Unknown";
        pthread_mutex_unlock( &remote_version_mutex );
        goto request_stop;
    }

    long response_code;
    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        ezylog_logerror( logger , "when getting remote version, response json error at line %d: %s" , error.line , error.text );
        remote_version = "Unknown";
        goto request_stop;
    } // json error

    if ( response_code != 200 )
    {
        ezylog_logerror( logger , "getting remote version failed: GitHub API responsed %ld" , response_code );
        ezylog_logerror( logger , "Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
        pthread_mutex_lock( &remote_version_mutex );
        remote_version = "Unknown";
        pthread_mutex_unlock( &remote_version_mutex );
        goto request_stop;
    } // response code is not 200 OK, error occured

    json_t* remote_version_jsonobj = json_array_get( root , 0 );
    remote_version_jsonobj = json_object_get( remote_version_jsonobj , "tag_name" );
    remote_version = ( char* ) malloc( 16 );
    strcpy( remote_version , json_string_value( remote_version_jsonobj ) );
    // get remote version from GitHub API response: response[0]["tag_name"]
    ezylog_logdebug( logger , "Latest remote version got: %s" , remote_version );

request_stop:
    curl_easy_cleanup( curl );
    free( response_data.ptr );
    json_decref( root );
    return;
}

bool is_local_latest(){
    if ( strcmp( remote_version , "Unknown" ) == 0 )
        return true;
    // remote version didn't get

    int local_major , local_minor , local_build;
    int remote_major , remote_minor , remote_build;
    sscanf( CGPTERM_VERSION , "v%d.%d.%d" , &local_major , &local_minor , &local_build );
    sscanf( remote_version , "v%d.%d.%d" , &remote_major , &remote_minor , &remote_build );
    long local = local_major * 10000 + local_minor * 100 + local_build;
    long remote = remote_major * 10000 + remote_minor * 100 + remote_build;
    return local < remote ? false : true;
}