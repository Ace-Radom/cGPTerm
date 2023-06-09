#ifndef _CURLFUNCTIONS_H_
#define _CURLFUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<jansson.h>

#include"ezylog.h"
#include"crich.h"
#include"utils.h"
#include"streamwrite.h"

#define GITHUB_REMOTE_VERSION_GET          0x00
#define OPENAI_NORMAL_REQUEST              0x01
#define OPENAI_STREAM_REQUEST              0x02
#define OPENAI_USAGE_GET_REQUEST           0x03
#define OPENAI_BACKGROUND_TITLE_GENERATION 0x04

typedef struct {
    unsigned char request_type;
    char* ptr;
    size_t size;
} curl_data_t;

size_t curl_write_callback_function( char* ptr , size_t size , size_t nmemb , void* userdata );
void SSE_event_handler( const char* SSEMSG );

#ifdef __cplusplus
}
#endif

#endif