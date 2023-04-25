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

#include<curl/curl.h>
#include<pthread.h>
#include<jansson.h>

#include"ezylog.h"
#include"utils.h"

typedef struct {
    char* endpoint;
    struct curl_slist* headers;
    json_t* messages;
    char* model;
    int tokens_limit;
    long long total_tokens_spent;
    int current_tokens;
    char* title;

    double credit_total_granted;
    double credit_total_used;
    double credit_used_this_month;
} openai_t;

extern openai_t* openai;

void openai_init();
void openai_send_chatrequest( const char* __usrmsg );
void openai_free();

#ifdef __cplusplus
}
#endif

#endif