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
#include"cdate.h"
#include"pthread_pool.h"
#include"cvector.h"

typedef struct {
    char* endpoint;
    struct curl_slist* headers;
    json_t* messages;
    char* model;
    double temperature;
    int tokens_limit;
    long total_tokens_spent;
    int current_tokens;
    char* title;

    double credit_total_granted;
    double credit_total_used;
    double credit_used_this_month;
    char* credit_plan;
} openai_t;

typedef struct {
    const char* msg;
    char* response;
} openai_datatransfer_t;

extern openai_t* openai;
extern bool request_working;
extern long HTTP_Response_code;

extern bool curl_request_abort_called;

void openai_init();
void openai_send_chatrequest( void* __data );
void openai_free();

int openai_set_model( char* __new_model );
void openai_set_temperature( double __new_temperature );
int openai_save_history( FILE* __f );
void openai_load_history( const char* __history_file );

void openai_get_usage_summary();

void openai_request_abort();

void openai_undo();
void openai_printlast();
void openai_msg_popback();

#ifdef __cplusplus
}
#endif

#endif