#include"openai_api.h"

openai_t* openai = NULL;
bool request_working = false;
long HTTP_Response_code = 0;

typedef struct {
    char* ptr;
    size_t size;
} curl_chat_data_t;

size_t write_callback_chat( char* ptr , size_t size , size_t nmemb , void* userdata ){
    curl_chat_data_t* response_data = ( curl_chat_data_t* ) userdata;
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

    openai -> current_tokens += count_tokens_message( prompt );
    ezylog_logdebug( logger , "prompt tokens: %ld" , openai -> current_tokens );
    // count prompt tokens and add to current_tokens

    curl_global_init( CURL_GLOBAL_ALL );
    // curl init

    return;
}

void openai_send_chatrequest( void* __data ){
    request_working = true;
    openai_datatransfer_t* data = ( openai_datatransfer_t* ) __data;
    const char* __usrmsg = data -> msg;
    // unpack transfer data

    CURL* curl;
    CURLcode res;
    curl_chat_data_t response_data = { NULL , 0 , false };
    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "[openai_send_chatrequest] -> curl init failed\n" );
        ezylog_logerror( logger , "curl init failed" );
        request_working = false;
        return;
    } // curl init error

    ezylog_loginfo( logger , "> %s" , __usrmsg );
    char* text;
    // return var

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
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , write_callback_chat );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        fprintf( stderr , "send request failed: %s\n" , curl_easy_strerror( res ) );
        ezylog_logerror( logger , "send request failed: %s" , curl_easy_strerror( res ) );
        text = NULL;
        goto request_stop;
    }

    ezylog_logdebug( logger , "Response got, total size: %ld" , response_data.size );

    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &HTTP_Response_code );
    // get api response code and parse it later

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        fprintf( stderr , "[openai_send_chatrequest] -> response json error at line %d: %s\n" , error.line , error.text );
        text = NULL;
        goto request_stop;
    } // json error
    
    if ( HTTP_Response_code / 100 == 4 )
    {
        json_t* response_errormsg = json_object_get( root , "error" );
        text = json_string_value( json_object_get( response_errormsg , "message" ) );
        ezylog_logerror( logger , "OpenAI API responsed Error: Code %ld, Message: \"%s\"" , HTTP_Response_code , text );
        ezylog_logdebug( logger , "GPT Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
        // because of unknown format problem (openai's error response contains \n), using json_dumps here
    } // parse response code, match 4xx errros
    else
    {
        json_t* response_msg = json_object_get( root , "choices" );
        response_msg = json_array_get( response_msg , 0 );
        response_msg = json_object_get( response_msg , "message" );
        json_array_append_new( openai -> messages , response_msg );
        text = json_string_value( json_object_get( response_msg , "content" ) );
        ezylog_loginfo( logger , "ChatGPT: %s" , text );
        ezylog_logdebug( logger , "GPT Response raw: %s" , response_data.ptr );
    } // response code 200 OK (most likely)

request_stop:
    curl_easy_cleanup( curl );
    free( request_data );
    free( response_data.ptr );
    data -> response = text;
    request_working = false;
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

void openai_msg_popback(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        return;
    } // here: normally msglist size can not be 0 because of the prompt message;
      // anyway when there's only prompt message in the list then it can not be pop.
    json_array_remove( openai -> messages , msglist_size - 1 );
    return;
}