#include"rwcfg/ini.h"

/**
 * @brief read config ini
 * 
 * @param __inipath config ini path
 * 
 * @throw 1 when cannot read config ini
 * 
 * @return 0 when success config ini successfully opened
*/
int rconfig( const char* __inipath ){
    config = iniparser_load( __inipath );
    if ( config == NULL )
    {
        fprintf( stderr , "[rconfig] -> cannot open config ini: %s\n " , __inipath );
        return 1;
    }

    OPENAI_API_KEY = ( char* ) malloc( 64 );
    CHAT_SAVE_PERFIX = ( char* ) malloc( 64 );

    strcpy( OPENAI_API_KEY , iniparser_getstring( config , "DEFAULT:OPENAI_API_KEY" , NULL ) );
    OPENAI_API_TIMEOUT = iniparser_getdouble( config , "DEFAULT:OPENAI_API_TIMEOUT" , 30.0 );
    // here: read AUTO_GENERATE_TITLE
    strcpy( CHAT_SAVE_PERFIX , iniparser_getstring( config , "DEFAULT:CHAT_SAVE_PERFIX" , NULL ) );
    // here: read LOG_LEVEL
    return 0;
}

/**
 * @brief close config ini
*/
void cconfig(){
    iniparser_freedict( config );
    return;
}