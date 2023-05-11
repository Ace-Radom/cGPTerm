#include"openai_api/curlfunctions.h"

char* stream_response_msg_only_buf = NULL;

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
    } // handle stream SSE event

    memcpy( &( response_data -> ptr[response_data->size] ) , ptr , realsize );
    response_data -> size += realsize;
    response_data -> ptr[response_data->size] = '\0';
    // copy response this turn

    // although it's possible that request data is handled in SSE handler functions, still build the whole requests and send it back
    // because: OpenAI API also responses json data when error occurs (RESPONSE_CODE / 100 == 4), and error must be parsed by request_post master thread
    // therefore: send back the whole response data is necessary (and still, request_post master thread need whole response to calc response size)

    return realsize;
}

void SSE_event_handler( const char* SSEMSG ){
    char* bpos;
    if ( ( bpos = strstr( SSEMSG , "data" ) ) == NULL )
        return;
    // not SSE event (maybe stream mode enabled, but error responsed)

    if ( request_working )
    {
        request_working = !request_working;
        while ( wait_msg_working );
    }
        
    // stop request working to end wait message in cli main process

    char* msg_json = ( char* ) malloc( strlen( SSEMSG ) + 1 );
    size_t counter = 0;
    for ( int i = 6 ; i < strlen( SSEMSG ) ; i++ )
        msg_json[counter++] = SSEMSG[i];
    msg_json[counter] = '\0';
    // copy SSE response json part

    if ( strcmp( msg_json , "[DONE]" ) == 0 )
        return;

    json_t* root;
    json_error_t error;
    root = json_loads( msg_json , 0 , &error );
    if ( !root )
    {
        fprintf( stderr , "[SSE_event_handler] -> response json at line %d: %s\n" , error.line , error.text );   
        ezylog_logerror( logger , "SSE response json error at line %d: %s" , error.line , error.text );
        ezylog_logerror( logger , "SSE Response raw: %s" , SSEMSG );
        return;
    }

    json_t* response_msg = json_object_get( root , "choices" );
    response_msg = json_array_get( response_msg , 0 );
    response_msg = json_object_get( response_msg , "delta" );
    if ( ( response_msg = json_object_get( response_msg , "content" ) ) != NULL )
    {
        printf( "%s" , json_string_value( response_msg ) );
        fflush( stdout );
        strcat( stream_response_msg_only_buf , json_string_value( response_msg ) );
    }

    ezylog_logdebug( logger , "SSE event: %s" , SSEMSG );
    free( msg_json );
    json_decref( root );
    return;
}