#include"rwcfg/ini.h"

int create_config( const char* __inipath ){
    FILE* Fcfg = fopen( __inipath , "w" );
    if ( Fcfg == NULL )
    {
        fprintf( stderr , "[create_config] -> create config file \"%s\" failed\n" , __inipath );
        return -1;
    }
    ini_t* _config = iniparser_load( __inipath );
    iniparser_set( _config , "DEFAULT"                     , NULL              ); // section DEFAULT
    iniparser_set( _config , "DEFAULT:OPENAI_API_KEY"      , NULL              );
    iniparser_set( _config , "DEFAULT:OPENAI_API_TIMEOUT"  , "30.0"            );
    iniparser_set( _config , "DEFAULT:AUTO_GENERATE_TITLE" , "True"            );
    iniparser_set( _config , "DEFAULT:CHAT_SAVE_PERFIX"    , "./chat_history_" );
    iniparser_set( _config , "DEFAULT:LOG_LEVEL"           , "INFO"            );
    iniparser_dump_ini( _config , Fcfg );
    iniparser_freedict( _config );
    fclose( Fcfg );
    return 0;
}

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
        return -1;
    }

    OPENAI_API_KEY = ( char* ) malloc( 64 );
    CHAT_SAVE_PERFIX = ( char* ) malloc( 64 );

    strcpy( OPENAI_API_KEY , iniparser_getstring( config , "DEFAULT:OPENAI_API_KEY" , NULL ) );
    OPENAI_API_TIMEOUT = iniparser_getdouble( config , "DEFAULT:OPENAI_API_TIMEOUT" , 30.0 );
    AUTO_GENERATE_TITLE = iniparser_getboolean( config , "DEFAULT:AUTO_GENERATE_TITLE" , true );
    strcpy( CHAT_SAVE_PERFIX , iniparser_getstring( config , "DEFAULT:CHAT_SAVE_PERFIX" , NULL ) );
    LOG_LEVEL = parse_str_to_priority( ( char* ) iniparser_getstring( config , "DEFAULT:LOG_LEVEL" , "INFO" ) );
    return 0;
}

int config_setstr( const char* __entry , const char* __value ){
    return iniparser_set( config , __entry , __value );
}

int config_setdouble( const char* __entry , double __value ){
    char* chgtemp = ( char* ) malloc( 16 );
    // double -> str change temp
    sprintf( chgtemp , "%2lf" , __value );
    int set_return = iniparser_set( config , __entry , chgtemp );
    free( chgtemp );
    return set_return;
}

int wconfig( const char* __inipath ){
    FILE* iniout = fopen( __inipath , "w" );
    if ( iniout == NULL )
    {
        fprintf( stderr , "[wconfig] -> Open config file \"%s\" failed\n" , __inipath );
        return -1;
    }
    iniparser_dump_ini( config , iniout );
    fclose( iniout );
    return 0;
}

/**
 * @brief close config ini
*/
void cconfig(){
    iniparser_freedict( config );
    return;
}