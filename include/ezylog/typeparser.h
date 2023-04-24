#ifndef _TYPEPARSER_H_
#define _TYPEPARSER_H_

#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#include<stdlib.h>

#include"elenum.h"

ezylog_priority_t parse_str_to_priority( char* __str );
char* parse_priority_to_str( ezylog_priority_t __p );

#endif