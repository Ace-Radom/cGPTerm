#include"openai_api/tiktokens.h"

/**
 * @note the input should be something like: {"role": "user", "content": prompt}
*/
long count_tokens_message( const json_t* __msg ){
    const char* role = json_string_value( json_object_get( __msg , "role" ) );
    const char* content = json_string_value( json_object_get( __msg , "content" ) );
    char* full_msg = ( char* ) malloc( strlen( role ) + strlen( content ) + 32 );
    sprintf( full_msg , "role: %s, content: %s" , role , content );
    long tokens = count_tokens( full_msg );
    free( full_msg );
    return tokens;
}