#include"openai_api/curlfunctions.h"

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

    if ( response_data -> request_type == OPENAI_STREAM_REQUEST )
    {
        char* temp = ( char* ) malloc( strlen( ptr ) + 1 );
        strcpy( temp , ptr );
        char* token = strtok( temp , "\n\n" );
        SSE_event_handler( token );
        while ( ( token = strtok( NULL , "\n\n" ) ) != NULL )
            SSE_event_handler( token );
        free( temp );
    } // handle stream sse event

    memcpy( &( response_data -> ptr[response_data->size] ) , ptr , realsize );
    response_data -> size += realsize;
    response_data -> ptr[response_data->size] = '\0';
    // copy response this turn
    return realsize;
}

void SSE_event_handler( const char* SSEMSG ){
    char* bpos;
    if ( ( bpos = strstr( SSEMSG , "data" ) ) == NULL )
        return;
    // not SSE event (maybe stream mode enabled, but error responsed)

    // ezylog_logdebug( logger , SSEMSG );
    return;
}