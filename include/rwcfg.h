#ifndef _RWCFG_H_
#define _RWCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"rwcfg/ini.h"

int setcfg_OPENAI_HOST( const char* __host );
int setcfg_OPENAI_API_KEY( const char* __api_key );
int setcfg_OPENAI_API_TIMEOUT( const double __timeout );
int setcfg_AUTO_GENERATE_TITLE();
int setcfg_CHAT_SAVE_PERFIX( const char* __perfix );
int setcfg_LOG_LEVEL( const char* __level );

#ifdef __cplusplus
}
#endif

#endif