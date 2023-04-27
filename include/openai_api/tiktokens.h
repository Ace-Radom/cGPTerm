#ifndef _TIKTOKENS_H_
#define _TIKTOKENS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<jansson.h>

extern long count_tokens_cl100k_base( const char* ptr );
// this is a rust FFI used to count tokens of a message with cl100k_base

long count_tokens_message( const json_t* __msg );

#ifdef __cplusplus
}
#endif

#endif