#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>

#include"rwcfg.h"
#include"argparse.h"
#include"utils.h"
#include"ezylog.h"
#include"openai_api.h"
#include"cli.hpp"

int main( int argc , char** argv ){
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
    args_parse( argc , argv );
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

    ezylog_logdebug( logger , "config set not triggered, start launching cGPTerm main service" );
    
    openai_init();
    ezylog_logdebug( logger , "openai service initialization complete" );

    printf( "Hi, welcome to chat with GPT. Type `/help` to display available commands.\n" );
    ezylog_loginfo( logger , "cGPTerm main service launch" );
    int CLI_returncode = start_CLI();
    // transfer into CXX - CLI is written in C++ because of unknown error with zh characters
    
    openai_free();

stopmain:
    cconfig();
    printf( "Goodbye\n" );
    ezylog_loginfo( logger , "cGPTerm master process shutting down..." );
    ezylog_close( logger );
    return 0;
    
}
