#include"rwcfg/parseinistr.h"

/**
 * @brief parse string to ezylog priority
 * 
 * @param __str string need to be parsed
 * 
 * @return Priority, when unrecognized priority str appeared return INFO
*/
ezylog_priority_t parse_str_to_elp( char* __str ){
    char* __strallupr = __str;
    for ( ; *__str != '\0' ; __str++ )
        *__str = toupper( *__str );
    if ( strcmp( __strallupr , "DEBUG" ) == 0 )
        return EZYLOG_PRIORITY_DEBUG;
    else if ( strcmp( __strallupr , "INFO" ) == 0 )
        return EZYLOG_PRIORITY_INFO;
    else if ( strcmp( __strallupr , "ERROR" ) == 0 )
        return EZYLOG_PRIORITY_ERROR;
    else if ( strcmp( __strallupr , "FATAL" ) == 0 )
        return EZYLOG_PRIORITY_FATAL;

    return EZYLOG_PRIORITY_INFO;
    // unrecognized priority, set to INFO
}