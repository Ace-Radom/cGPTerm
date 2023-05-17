#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<libgen.h>

#include<curl/curl.h>
#include<jansson.h>

bool package_connected = false;
char* remote_version = NULL;
char* local_version = NULL;
long response_code = 0;

typedef struct {
    char* ptr;
    size_t size;
} curl_data_t;

size_t curl_write_callback_function( char* ptr , size_t size , size_t nmemb , void* userdata );

size_t curl_download_callback_function( void* ptr , size_t size , size_t nmemb , FILE* stream );
 
int curl_progress_callback_function( char* progress_data , double dltotal , double dlnow , double ultotal , double ulnow );

bool is_local_latest();
 
int main( int argc, char** argv )
{
    printf( "cGPTerm Updater %s\n" , UPDATER_VERSION );

    char updater_path[PATH_MAX] = { 0 };
    char* install_path = NULL;
    ssize_t readlink_path_length = readlink( "/proc/self/exe" , updater_path , sizeof( updater_path ) - 1 );
    if ( readlink_path_length != -1 )
    {
        updater_path[readlink_path_length] = '\0';
        install_path = dirname( updater_path );
    }
    else
    {
        fprintf( stderr , "Cannot find cGPTerm Updater installed path, exit\n" );
        return 128;
    }
    // get install path

    curl_global_init( CURL_GLOBAL_ALL );
    curl_version_info_data *curl_version_data = curl_version_info( CURLVERSION_NOW );
    char* curl_userangel_data = ( char* ) malloc( 16 );
    sprintf( curl_userangel_data , "curl/%s" , curl_version_data -> version );
    // curl init

    printf( "Asking latest remote version...\n" );

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { NULL , 0 };

    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "CURL init failed when getting remote version, exit\n" );
        curl_global_cleanup();
        return 128;
    }

    curl_easy_setopt( curl , CURLOPT_URL , "https://api.github.com/repos/Ace-Radom/cGPTerm/releases" );
    curl_easy_setopt( curl , CURLOPT_HTTPGET , 1L );
    curl_easy_setopt( curl , CURLOPT_USERAGENT , curl_userangel_data );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT , 10 );
    // make curl request

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        fprintf( stderr , "Get remote version failed: %s\n" , curl_easy_strerror( res ) );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        return 128;
    }

    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        fprintf( stderr , "When getting remote version, response json error at line %d: %s (see ./github_res_dump.json to see response raw)" , error.line , error.text );
        FILE* ferr = fopen( "./github_res_dump.json" , "w" );
        fputs( response_data.ptr , ferr );
        fclose( ferr );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        return 128;
    } // dump response, exit

    if ( response_code != 200 )
    {
        fprintf( stderr , "Getting remote version failed: GitHub API responsed %ld" , response_code );
        FILE* ferr = fopen( "./github_res_dump.json" , "w" );
        fputs( response_data.ptr , ferr );
        fclose( ferr );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        return 128;
    } // response not 200 OK

    json_t* remote_version_jsonobj = json_array_get( root , 0 );
    remote_version_jsonobj = json_object_get( remote_version_jsonobj , "tag_name" );
    remote_version = ( char* ) malloc( 16 );
    strcpy( remote_version , json_string_value( remote_version_jsonobj ) );
    // copy remote version
    json_decref( root );
    curl_easy_cleanup( curl );

    printf( "Latest remote version: %s\n" , remote_version );

    bool cgpterm_installed = true;

    FILE* cgpterm_version_pipe = NULL;
    char* cgpterm_version_cmd = ( char* ) malloc( PATH_MAX );
    sprintf( cgpterm_version_cmd , "%s/cgpterm" , install_path );
    if ( access( cgpterm_version_cmd , F_OK ) == -1 )
    {
        perror( "Cannot ask cGPTerm local version" );
        printf( "If seems that you haven't installed cGPTerm, do you want to install it? [Y/n] " );
        char confirm;
        scanf( " %s" , &confirm );
        if ( confirm == 'Y' )
        {
            free( cgpterm_version_cmd );
            cgpterm_installed = false;
            goto install;
        }
        printf( "Abort\n" );
        free( remote_version );
        free( cgpterm_version_cmd );
        curl_global_cleanup();
        return 1;
    } // check if cGPTerm exists (cgpterm_version_cmd here is still the cGPTerm path)
    strcat( cgpterm_version_cmd , " --version" );
    cgpterm_version_pipe = popen( cgpterm_version_cmd , "r" );
    if ( cgpterm_version_pipe == NULL )
    {
        fprintf( stderr , "Cannot get cGPTerm local version, exit\n" );
        free( remote_version );
        curl_global_cleanup();
        return 128;
    }
    local_version = ( char* ) malloc( 16 );
    fgets( local_version , 16 , cgpterm_version_pipe );
    pclose( cgpterm_version_pipe );
    free( cgpterm_version_cmd );
    // get local version

    printf( "Local version: %s\n" , local_version );

    if ( is_local_latest() )
    {
        printf( "cGPTerm on this device is already the latest cGPTerm release\nNothing to do, exit\n" );
        free( remote_version );
        free( local_version );
        curl_global_cleanup();
        return 1;
    } // local is the latest

    printf( "Update available, do you want to continue? [Y/n] " );
    char confirm;
    scanf( " %c" , &confirm );

    if ( confirm != 'Y' )
    {
        printf( "Abort\n" );
        free( remote_version );
        free( local_version );
        curl_global_cleanup();
        return 1;
    } // abort

install:

    if ( cgpterm_installed )
        printf( "Start update cGPTerm to %s\n" , remote_version );
    else
        printf( "Start install cGPTerm %s\n" , remote_version );

    char* download_url = ( char* ) malloc( 512 );
    char* download_tar_name = ( char* ) malloc( PATH_MAX );
    sprintf( download_url , "https://github.com/Ace-Radom/cGPTerm/releases/download/%s/cGPTerm_%s.tar.gz" , remote_version , remote_version );
    sprintf( download_tar_name , "%s/cGPTerm_%s.tar.gz" , install_path , remote_version );

    FILE* tar = NULL;
    tar = fopen( download_tar_name , "wb" );
    if ( tar == NULL )
    {
        perror( "Create package file failed" );
        free( remote_version );
        free( local_version );
        free( download_url );
        free( download_tar_name );
        curl_global_cleanup();
        return 128;
    }
    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "CURL init failed when downloading latest release package, exit\n" );
        free( remote_version );
        free( local_version );
        free( download_url );
        free( download_tar_name );
        curl_global_cleanup();
        return 128;
    }

    curl_easy_setopt( curl , CURLOPT_URL , download_url );
    curl_easy_setopt( curl , CURLOPT_FOLLOWLOCATION , 1L );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , tar );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_download_callback_function );
    curl_easy_setopt( curl , CURLOPT_NOPROGRESS , 0L );
    curl_easy_setopt( curl , CURLOPT_PROGRESSFUNCTION , curl_progress_callback_function );
    curl_easy_setopt( curl , CURLOPT_PROGRESSDATA , download_tar_name );
    package_connected = false;
    // make curl request

    res = curl_easy_perform( curl );
    printf( "\n" );
    if ( res != CURLE_OK )
    {
        fprintf( stderr , "Download latest release package failed: %s\n" , curl_easy_strerror( res ) );
        fclose( tar );
        remove( download_tar_name );
        free( remote_version );
        free( local_version );
        free( download_url );
        free( download_tar_name );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        return 128;
    }

    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );
    if ( response_code != 200 )
    {
        fprintf( stderr , "Getting remote version failed: GitHub API responsed %ld" , response_code );
        fclose( tar );
        remove( download_tar_name );
        free( remote_version );
        free( local_version );
        free( download_url );
        free( download_tar_name );
        curl_easy_cleanup( curl );
        curl_global_cleanup();
        return 128;
    }

    printf( "Package download complete\n" );

    fclose( tar );
    curl_easy_cleanup( curl );

    if ( cgpterm_installed )
    {
        printf( "Collecting installed cGPTerm files...\n" );

        FILE* dfl = NULL;
        char* dfl_path = ( char* ) malloc( PATH_MAX );
        sprintf( dfl_path , "%s/DFL" , install_path );
        dfl = fopen( dfl_path , "r" );
        if ( dfl == NULL )
        {
            perror( "Open download file list failed" );
            remove( download_tar_name );
            free( remote_version );
            free( local_version );
            free( download_url );
            free( download_tar_name );
            curl_global_cleanup();
            return 128;
        }
        char* files = ( char* ) malloc( 64 );
        while ( ( fgets( files , 64 , dfl ) ) != NULL )
        {
            char* this_file_path = ( char* ) malloc( PATH_MAX );
            sprintf( this_file_path , "%s/%s" , install_path , files );
            this_file_path[strlen(this_file_path)-1] = '\0';
            // erase endline
            printf( "Remove %s\n" , this_file_path );
            remove( this_file_path );
            free( this_file_path );
        } // remove old files
        free( files );
        fclose( dfl );
        free( dfl_path );
    }

    printf( "Install new cGPTerm release\nUnpacking package...\n" );

    char* install_cmd = ( char* ) malloc( PATH_MAX + 64 );
    sprintf( install_cmd , "tar -xzvf %s -C %s" , download_tar_name , install_path );
    int sysrc = system( install_cmd );
    free( install_cmd );
    if ( sysrc != 0 )
    {
        fprintf( stderr , "Unpack package failed\n" );
        remove( download_tar_name );
        free( remote_version );
        free( local_version );
        free( download_url );
        free( download_tar_name );
        curl_global_cleanup();
        return 128;
    } // tar return not 0

    if ( cgpterm_installed )
        printf( "Unpack success\ncGPTerm has been successfully updated to %s\n" , remote_version );
    else
        printf( "Unpack success\ncGPTerm %s has been successfully installed to %s\n" , remote_version , install_path );
    remove( download_tar_name );
    free( remote_version );
    free( local_version );
    free( download_url );
    free( download_tar_name );

    curl_global_cleanup();
    return 0;
}

size_t curl_write_callback_function( char* ptr , size_t size , size_t nmemb , void* userdata ){
    curl_data_t* response_data = ( curl_data_t* ) userdata;
    // link callback and main request thread, transfer data
    size_t realsize = size * nmemb;

    response_data -> ptr = realloc( response_data -> ptr , response_data -> size + realsize + 1 );
    if ( response_data -> ptr == NULL )
    {
        fprintf( stderr , "[openai_api->write_callback] -> realloc data transfer buf failed\n" );
        return 0;
    } // realloc failed

    memcpy( &( response_data -> ptr[response_data->size] ) , ptr , realsize );
    response_data -> size += realsize;
    response_data -> ptr[response_data->size] = '\0';
    // copy response this turn

    return realsize;
}

size_t curl_download_callback_function( void* ptr , size_t size , size_t nmemb , FILE* stream ){
    return fwrite( ptr , size , nmemb , stream );
} 

int curl_progress_callback_function( char* progress_data , double dltotal , double dlnow , double ultotal , double ulnow ){
    if ( dltotal == 0 || dlnow == 0 )
        printf( "\rConnecting..." );
    else
    {
        if ( !package_connected )
        {
            package_connected = true;
            printf( "\r\033[2K\rConnected, downloading package to %s...\n" , progress_data );
        }
        printf( "\rProgress: %.4lf %%" , dlnow * 100.0 / dltotal );
    }
    fflush( stdout );
    return 0;
}

bool is_local_latest(){
    int local_major , local_minor , local_build;
    int remote_major , remote_minor , remote_build;
    sscanf( CGPTERM_VERSION , "v%d.%d.%d" , &local_major , &local_minor , &local_build );
    sscanf( remote_version , "v%d.%d.%d" , &remote_major , &remote_minor , &remote_build );
    long local = local_major * 10000 + local_minor * 100 + local_build;
    long remote = remote_major * 10000 + remote_minor * 100 + remote_build;
    return local < remote ? false : true;
}