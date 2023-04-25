#include"openai_api.h"

openai_t* openai = NULL;

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

void openai_free(){
    curl_slist_free_all( openai -> headers );
    json_decref( openai -> messages );
    free( openai -> model );
    if ( openai -> title )
        free( openai -> title );
    free( openai );
    return;
}