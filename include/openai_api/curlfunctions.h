#ifndef _CURLFUNCTIONS_H_
#define _CURLFUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

typedef struct {
    char* ptr;
    size_t size;
} curl_data_t;

size_t curl_write_callback_function( char* ptr , size_t size , size_t nmemb , void* userdata );

#ifdef __cplusplus
}
#endif

#endif