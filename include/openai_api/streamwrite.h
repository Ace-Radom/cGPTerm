#ifndef _STREAMWRITE_H_
#define _STREAMWRITE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/ioctl.h>

#include"crich.h"
#include"utils.h"

extern char* stream_response_msg_only_buf;

void write_stream( const char* __msg );

#ifdef __cplusplus
}
#endif

#endif