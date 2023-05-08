#include"ezylog/typeparser.h"

/**
 * @brief parse string to ezylog priority
 * 
 * @param __str string need to be parsed
 * 
 * @return Priority, when unrecognized priority str appeared return INFO
*/
ezylog_priority_t parse_str_to_priority( const char* __str ){
    char* str = ( char* ) malloc( strlen( __str ) );
    strcpy( str , __str );
    char* strallupr = str;
    for ( ; *str != '\0' ; str++ )
        *str = toupper( *str );
    if ( strcmp( strallupr , "DEBUG" ) == 0 )
    {
        free( strallupr );
        return EZYLOG_PRIORITY_DEBUG;
    } 
    else if ( strcmp( strallupr , "INFO" ) == 0 )
    {
        free( strallupr );
        return EZYLOG_PRIORITY_INFO;
    }  
    else if ( strcmp( strallupr , "ERROR" ) == 0 )
    {
        free( strallupr );
        return EZYLOG_PRIORITY_ERROR;
    }
    else if ( strcmp( strallupr , "FATAL" ) == 0 )
    {
        free( strallupr );
        return EZYLOG_PRIORITY_FATAL;
    }
    free( strallupr );
    return EZYLOG_PRIORITY_INFO;
    // unrecognized priority, set to INFO
}

char* parse_priority_to_str( ezylog_priority_t __p ){
    switch ( __p ) {
        case EZYLOG_PRIORITY_DEBUG: return "DEBUG";
        case EZYLOG_PRIORITY_INFO:  return "INFO";
        case EZYLOG_PRIORITY_ERROR: return "ERROR";
        case EZYLOG_PRIORITY_FATAL: return "FATAL";
    }
    return NULL;
}