#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include<curl/curl.h>
#include<jansson.h>

bool package_connected = false;

typedef struct {
    char* ptr;
    size_t size;
} curl_data_t;

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
 
int curl_progress_function( char* progress_data , double dltotal , double dlnow , double ultotal , double ulnow ){
    if ( dltotal == 0 || dlnow == 0 )
        printf( "\rConnecting..." );
    else
    {
        if ( !package_connected )
        {
            package_connected = true;
            printf( "\r\033[2K\rConnected, downloading PowerShell-7.3.4-win.msixbundle...\n" );
        }
        printf( "\rProgress: %.4lf %%" , dlnow * 100.0 / dltotal );
    }
    fflush( stdout );
    return 0;
}
 
int main( int argc, char **argv )
{
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
        fprintf( stderr , "curl init failed when getting remote version, exit\n" );
        return 1;
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
        fprintf( stderr , "Get remote version failed.\n" );
        return 1;
    }

    int response_code;
    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        fprintf( stderr , "when getting remote version, response json error at line %d: %s (see ./github_res_dump.json to see response raw)" , error.line , error.text );
        FILE* ferr = fopen( "./github_res_dump.json" , "w" );
        fputs( response_data.ptr , ferr );
        fclose( ferr );
        return 1;
    } // dump response, exit

    if ( response_code != 200 )
    {
        fprintf( stderr , "getting remote version failed: GitHub API responsed %d" , response_code );
        FILE* ferr = fopen( "./github_res_dump.json" , "w" );
        fputs( response_data.ptr , ferr );
        fclose( ferr );
        return 1;
    } // response not 200 OK

    json_t* remote_version_jsonobj = json_array_get( root , 0 );
    remote_version_jsonobj = json_object_get( remote_version_jsonobj , "tag_name" );
    char* remote_version = ( char* ) malloc( 16 );
    strcpy( remote_version , json_string_value( remote_version_jsonobj ) );
    // copy remote version
    json_decref( root );

    printf( "Latest remote version: %s\n" , remote_version );

    char *url = "https://github.com/PowerShell/PowerShell/releases/download/v7.3.4/PowerShell-7.3.4-win.msixbundle";
    char *progress_data = "* ";


    // FILE *outfile;
    // curl = curl_easy_init();
    // if(curl)
    // {
    //     outfile = fopen("tar", "wb");
 
    //     curl_easy_setopt(curl, CURLOPT_URL, url);
    //     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION , 1L );
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_download_callback_function );
    //     curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L );
    //     curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, curl_progress_function );
    //     curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress_data);
    //     package_connected = false;
    //     res = curl_easy_perform(curl);
 
    //     fclose(outfile);
    //     /* always cleanup */
    //     curl_easy_cleanup(curl);
    // }
    curl_global_cleanup();
    return 0;
}