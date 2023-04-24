#ifndef _RWCFG_H_
#define _RWCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"rwcfg/ini.h"

int setcfg_OPENAI_API_KEY( char* __api_key );
int setcfg_OPENAI_API_TIMEOUT( double __timeout );
int setcfg_AUTO_GENERATE_TITLE();
int setcfg_CHAT_SAVE_PERFIX( char* __perfix );
int setcfg_LOG_LEVEL( char* __level );

#ifdef __cplusplus
}
#endif

#endif