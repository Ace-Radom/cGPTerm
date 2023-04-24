#include"rwcfg.h"

int setcfg_OPENAI_API_KEY( char* __api_key ){
    return config_setstr( "DEFAULT:OPENAI_API_KEY" , __api_key );
}

int setcfg_OPENAI_API_TIMEOUT( double __timeout ){
    return config_setdouble( "DEFAULT:OPENAI_API_TIMEOUT" , __timeout );
}

int setcfg_AUTO_GENERATE_TITLE(){
    if ( AUTO_GENERATE_TITLE )
        return config_setstr( "DEFAULT:AUTO_GENERATE_TITLE" , "False" );
    else
        return config_setstr( "DEFAULT:AUTO_GENERATE_TITLE" , "True" );
}

int setcfg_CHAT_SAVE_PERFIX( char* __perfix ){
    return config_setstr( "DEFAULT:CHAT_SAVE_PERFIX" , __perfix );
}

int setcfg_LOG_LEVEL( char* __level ){
    return config_setstr( "DEFAULT:LOG_LEVEL" , parse_priority_to_str( parse_str_to_priority( __level ) ) );
}