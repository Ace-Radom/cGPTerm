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
#include"ctiktoken.hpp"

#define GEN_TITLE_PROMPT "Generate title shorter than 10 words for the following content in content's language. The tilte contains ONLY words. DO NOT include line-break."

typedef struct {
    char* endpoint;
    struct curl_slist* headers;
    json_t* messages;
    char* model;
    double temperature;
    bool stream_mode;
    int tokens_limit;
    long total_tokens_spent;
    int current_tokens;
    char* title;

    double credit_total_granted;
    double credit_total_used;
    double credit_used_this_month;
    const char* credit_plan;
} openai_t;

typedef struct {
    char* msg;
    const char* response;
} openai_datatransfer_t;

extern openai_t* openai;
extern long HTTP_Response_code;

extern bool curl_request_abort_called;

extern CURL* title_background_generation_curl;

void openai_init();
void* openai_send_chatrequest( void* __data );
void openai_free();

void openai_count_tokens();

int openai_set_model( char* __new_model );
void openai_set_prompt( char* __new_prompt );
void openai_set_temperature( double __new_temperature );
int openai_save_history( FILE* __f );
void openai_load_history( const char* __history_file );

void* openai_get_usage_summary();
void* openai_generate_title( void* __data );

void openai_request_abort();

void openai_undo();
void openai_delete_first();
void openai_delete_all();
size_t openai_get_message_list_length();
const char* openai_getfirst();
const char* openai_getlast();
void openai_msg_popback();

#ifdef __cplusplus
}
#endif

#endif