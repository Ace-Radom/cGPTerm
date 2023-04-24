#ifndef _INI_H_
#define _INI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include"iniparser.h"
#include"utils.h"
#include"ezylog/typeparser.h"

int rconfig( const char* __inipath );
void cconfig();

#ifdef __cplusplus
}
#endif

#endif