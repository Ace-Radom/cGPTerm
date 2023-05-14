#ifndef _TIKTOKENS_H_
#define _TIKTOKENS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<jansson.h>

#include"ctiktoken.hpp"

long count_tokens_message( const json_t* __msg );

#ifdef __cplusplus
}
#endif

#endif