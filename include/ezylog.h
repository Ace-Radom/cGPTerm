#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#include"ezylog/layout.h"
#include"ezylog/elenum.h"

#define NOTMSG_IN_LOG_MAX_LENGTH 128
#define LOG_MSG_MAX_LENGTH 2048
#define EL_MUTEX_LOCK_FAILED 1
#define EL_PT_BELOW_SET      2

typedef struct {
    char* __name[32];
    char* __layout[32];
    FILE* __f;
    ezylog_priority_t __pt;
    pthread_mutex_t __mutex;
} ezylog_logger_t;

ezylog_logger_t* ezylog_init( const char* __name , const char* __layout , const char* __logfile , const ezylog_logmode_t __mode , const ezylog_priority_t __pt );
int ezylog_logdebug( ezylog_logger_t* __logger , const char* __format , ... );
int ezylog_loginfo( ezylog_logger_t* __logger , const char* __format , ... );
int ezylog_logerror( ezylog_logger_t* __logger , const char* __format , ... );
int ezylog_logfatal( ezylog_logger_t* __logger , const char* __format , ... );

void ezylog_chglayout( ezylog_logger_t* __logger , const char* __newlayout );
void ezylog_chgpriority( ezylog_logger_t* __logger , ezylog_priority_t __newpt );

void ezylog_close( ezylog_logger_t* __logger );

#ifdef __cplusplus
}
#endif

#endif