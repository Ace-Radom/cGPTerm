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

void date_add_day( cdate_t* __date , int __delta ){
    struct tm date_tm = { 0 };
    date_tm.tm_year = __date -> year - 1900;
    date_tm.tm_mon = __date -> month - 1;
    date_tm.tm_mday = __date -> day;
    time_t ts = mktime( &date_tm );
    ts += ( __delta * 86400 );
    char* newdate = ( char* ) malloc( 40 );
    strftime( newdate , 40 , "%Y-%m-%d" , localtime( &ts ) );
    sscanf( newdate , "%d-%d-%d" , &__date -> year , &__date -> month , &__date -> day );
    free( newdate );
    return;
}

long date_diff( cdate_t __d1 , cdate_t __d2 ){
    long m1 = ( __d1.month + 9 ) % 12;
    long y1 = __d1.year - m1 / 10;
    long d1 = y1 * 365 + y1 / 4 - y1 / 100 + y1 / 400 + ( m1 * 306 + 5 ) / 10 + ( __d1.day - 1 );
    long m2 = ( __d2.month + 9 ) % 12;
    long y2 = __d2.year - m2 / 10;
    long d2 = y2 * 365 + y2 / 4 - y2 / 100 + y2 / 400 + ( m2 * 306 + 5 ) / 10 + ( __d2.day - 1 );

    return ( d2 - d1 );
}