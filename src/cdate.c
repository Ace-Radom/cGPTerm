#include"cdate.h"

void get_today_date( cdate_t* __d ){
    char* curtime = ( char* ) malloc( 40 );
    struct timeval tv;
    gettimeofday( &tv , NULL );
    time_t curtime_ts = tv.tv_sec;
    strftime( curtime , 40 , "%Y-%m-%d" , localtime( &curtime_ts ) );
    sscanf( curtime , "%d-%d-%d" , &__d -> year , &__d -> month , &__d -> day );
    free( curtime );
    return;
}

char* parse_date( cdate_t __date ){
    char* date = ( char* ) malloc( 16 );
    sprintf( date , "%04d-%02d-%02d" , __date.year , __date.month , __date.day );
    return date;
}