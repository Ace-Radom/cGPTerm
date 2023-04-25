#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>    
#include<time.h>
#include<sys/time.h>

#include"elenum.h"
#include"typeparser.h"

typedef struct {
    char type;
    char specifer;
} ezylog_format_t;

char* get_curtime( unsigned short int __accuracy );

void parse_layout( char* __s , const char* __layout , const char* __time , const char* __logger , const char* __msg  , const ezylog_priority_t __priority , const char* __addimsg );

#ifdef __cplusplus
}
#endif

#endif