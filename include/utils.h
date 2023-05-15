#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdbool.h>
#include<termios.h>

#include<pthread.h>

#include"ezylog.h"
#include"openai_api.h"
#include"iniparser.h"

typedef dictionary ini_t;

extern ezylog_logger_t* logger;
extern ini_t* config;

extern char* OPENAI_HOST;
extern char* OPENAI_API_KEY;
extern double OPENAI_API_TIMEOUT;
extern bool AUTO_GENERATE_TITLE;
extern char* CHAT_SAVE_PERFIX;
extern ezylog_priority_t LOG_LEVEL;

extern char* usrhome;       // ~
extern char* cfgdir;        // ~/.cgpterm
extern char* logpath;       // ~/.cgpterm/chat.log
extern char* cfginipath;    // ~/.cgpterm/config.ini

extern char* remote_version;
extern pthread_mutex_t remote_version_mutex;

extern char* curl_userangel_data;

extern bool raw_mode_enable;

extern bool request_working;
extern bool wait_msg_working;

#ifdef __cplusplus
}
#endif

#endif