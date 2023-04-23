#include"ezylog/layout.h"

const ezylog_format_t formats[] = {
    { 's'  , 'T'  }, // time
    { 's'  , 'l'  }, // logger
    { 's'  , 'm'  }, // message
    { 's'  , 'p'  }, // priority
    { 's'  , 'P'  }, // priority (with auto tab)
    { 's'  , 'a'  }, // additional message
    { '\0' , '\0' }
};

char* get_curtime( unsigned short int __accuracy ){
    char* curtime = ( char* ) malloc( 40 );
    char* buf = ( char* ) malloc( 30 );
    struct timeval tv;
    gettimeofday( &tv , NULL );
    time_t curtime_ts = tv.tv_sec;
    // get current time stamp
    strftime( buf , 30 , "%Y-%m-%d %T" , localtime( &curtime_ts ) );
    if ( __accuracy == 0 )
        sprintf( curtime , "%s.%ld" , buf , tv.tv_usec );
        // no microseconds accuracy require (default: 6bit)
    else
    {
        int accuarcy_div = pow( 10 , 6 - __accuracy );
        sprintf( curtime , "%s.%ld" , buf , tv.tv_usec / accuarcy_div );
        // microseconds accuracy require (1~6bit)
    }
    return curtime;
}

char* parse_priority( ezylog_priority_t __p ){
    switch ( __p ) {
        case EZYLOG_PRIORITY_DEBUG: return "DEBUG";
        case EZYLOG_PRIORITY_INFO:  return "INFO";
        case EZYLOG_PRIORITY_ERROR: return "ERROR";
        case EZYLOG_PRIORITY_FATAL: return "FATAL";
    }
    return NULL;
}

void parse_layout( char* __s , const char* __layout , const char* __time , const char* __logger , const char* __msg  , const ezylog_priority_t __priority , const char* __addimsg ){

    const char *p = __layout;
    char ch;
    while ( ( ch = *p++ ) != '\0' )
    {
        if ( ch == '%' )
        {
            char specifier = *p++;
            // after %, there should be a specifier
            ezylog_format_t format = { 0 };
            for ( int i = 0 ; formats[i].type != '\0' ; i++ )
            {
                if ( specifier == formats[i].specifer )
                {
                    format = formats[i];
                    break;
                }
            }
            // search specifer in format list
            void* value = malloc( sizeof( void* ) );
            switch ( format.specifer ) {
                case 'T': value = ( void* ) __time;    break;
                case 'l': value = ( void* ) __logger;  break;
                case 'm': value = ( void* ) __msg;     break;
                case 'a': value = ( void* ) __addimsg; break;

                case 'p':
                    value = ( void* ) parse_priority( __priority );
                    break;
                case 'P':
                    sprintf( ( char* ) value , "%-5s" , parse_priority( __priority ) );
                    break;
                
                default: // unrecognizable format
                    sprintf( __s , "%%%c" , specifier );
                    free( value );
                    break;
            }
            if ( format.type == 's' )
            {
                sprintf( __s , "%s" , ( char* ) value );
            }
            __s += strlen( __s );
        }
        else
        {
            *__s++ = ch;
            // not specifier
        }
    }
    *__s = '\0';
    return;
}