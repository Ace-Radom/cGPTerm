#include"openai_api.h"

openai_t* openai = NULL;

size_t write_callback_chat( char* ptr , size_t size , size_t nmemb , void* userdata ){
    size_t realsize = size * nmemb;
    json_t* root;
    json_error_t error;

    root = json_loads( ptr , 0 , &error );
    if ( !root )
    {
        fprintf( stderr , "[openai_api->write_callback] -> response json error at line %d: %s\n" , error.line , error.text );
        return 0;
    } // json error

    json_t* response_msg = json_object_get( root , "choices" );
    response_msg = json_array_get( response_msg , 0 );
    response_msg = json_object_get( response_msg , "message" );
    json_array_append_new( openai -> messages , response_msg );
    const char* text = json_string_value( json_object_get( response_msg , "content" ) );
    printf( "ChatGPT:\n%s\n" , text );
    ezylog_loginfo( logger , "ChatGPT: %s" , text );
    ezylog_logdebug( logger , "GPT Response raw: %s" , ptr );
    return realsize;
}

void openai_init(){
    openai = ( openai_t* ) malloc( sizeof( openai_t ) );
    openai -> endpoint = "https://api.openai.com/v1/chat/completions";
    openai -> headers = NULL;
    openai -> messages = json_array();
    openai -> model = ( char* ) malloc( 64 );
    strcpy( openai -> model , "gpt-3.5-turbo" );
    openai -> tokens_limit = 4096;
    openai -> total_tokens_spent = 0;
    openai -> current_tokens = 0;
    openai -> title = NULL;
    openai -> credit_total_granted = 0.0;
    openai -> credit_total_used = 0.0;
    openai -> credit_used_this_month = 0.0;
    // basic init

    char* authorization_bearer_header_str = ( char* ) malloc( 128 );
    sprintf( authorization_bearer_header_str , "%s%s" , "Authorization: Bearer " , OPENAI_API_KEY );
    openai -> headers = curl_slist_append( openai -> headers , "Content-Type: application/json" );
    openai -> headers = curl_slist_append( openai -> headers , authorization_bearer_header_str );
    // headers init

    json_t* prompt = json_object();
    json_object_set_new( prompt , "role" , json_string( "system" ) );
    json_object_set_new( prompt , "content" , json_string( "You are a helpful assistant. Knowledge cutoff: 2021-09" ) );
    json_array_append_new( openai -> messages , prompt );
    // prompt init

    curl_global_init( CURL_GLOBAL_ALL );
    // curl init

    return;
}

void openai_send_chatrequest( const char* __usrmsg ){
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "[openai_send_chatrequest] -> init curl failed\n" );
        return;
    }

    ezylog_loginfo( logger , "> %s" , __usrmsg );

    json_t* new_usrmsg = json_object();
    json_object_set_new( new_usrmsg , "role" , json_string( "user" ) );
    json_object_set_new( new_usrmsg , "content" , json_string( __usrmsg ) );
    json_array_append_new( openai -> messages , new_usrmsg );
    // append new user msg to messages
    json_t* request_json_root = json_object();
    json_object_set_new( request_json_root , "model" , json_string( openai -> model ) );
    json_object_set_new( request_json_root , "messages" , openai -> messages );
    char* request_data = json_dumps( request_json_root , JSON_COMPACT );
    ezylog_logdebug( logger , "Request raw: %s" , request_data );
    // make request data

    curl_easy_setopt( curl , CURLOPT_URL , openai -> endpoint );
    curl_easy_setopt( curl , CURLOPT_HTTPHEADER , openai -> headers );
    curl_easy_setopt( curl , CURLOPT_POSTFIELDS , request_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , write_callback_chat );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        fprintf( stderr , "send request failed: %s\n" , curl_easy_strerror( res ) );
    }
    curl_easy_cleanup( curl );
    free( request_data );
    return;
}

void openai_free(){
    curl_slist_free_all( openai -> headers );
    json_decref( openai -> messages );
    free( openai -> model );
    if ( openai -> title )
        free( openai -> title );
    free( openai );
    curl_global_cleanup();
    return;
}