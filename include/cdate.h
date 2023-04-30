#ifndef _CDATE_H_
#define _CDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>

typedef struct {
    unsigned int year;
    unsigned int month;
    unsigned int day;
} cdate_t;

void get_today_date( cdate_t* __d );
char* parse_date( cdate_t __date );
void date_add_day( cdate_t* __date , int __delta );
long date_diff( cdate_t __d1 , cdate_t __d2 );

#ifdef __cplusplus
}
#endif

#endif