#ifndef _PARSEINISTR_H_
#define _PARSEINISTR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<string.h>
#include<ctype.h>
#include<stdbool.h>

#include"ezylog.h"

ezylog_priority_t parse_str_to_elp( char* __str );

#ifdef __cplusplus
}
#endif

#endif