#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdbool.h>

#include"ezylog.h"
#include"openai_api.h"
#include"iniparser.h"

typedef dictionary ini_t;

extern ezylog_logger_t* logger;
extern ini_t* config;

extern char* OPENAI_API_KEY;
extern double OPENAI_API_TIMEOUT;
extern bool AUTO_GENERATE_TITLE;
extern char* CHAT_SAVE_PERFIX;
extern ezylog_priority_t LOG_LEVEL;

#ifdef __cplusplus
}
#endif

#endif