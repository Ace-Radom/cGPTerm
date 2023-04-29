#ifndef _OPENAI_API_H_
#define _OPENAI_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<errno.h>

#include<curl/curl.h>
#include<pthread.h>
#include<jansson.h>

#include"ezylog.h"
#include"utils.h"
#include"openai_api/tiktokens.h"
#include"openai_api/curlfunctions.h"
#include"crich.h"

typedef struct {
    char* endpoint;
    struct curl_slist* headers;
    json_t* messages;
    char* model;
    int tokens_limit;
    long total_tokens_spent;
    int current_tokens;
    char* title;

    double credit_total_granted;
    double credit_total_used;
    double credit_used_this_month;
} openai_t;

typedef struct {
    const char* msg;
    char* response;
} openai_datatransfer_t;

extern openai_t* openai;
extern bool request_working;
extern long HTTP_Response_code;

void openai_init();
void openai_send_chatrequest( void* __data );
void openai_free();

int openai_set_model( char* __new_model );
int openai_save_history( FILE* __f );
void openai_load_history( const char* __history_file );

void openai_msg_popback();

#ifdef __cplusplus
}
#endif

#endif