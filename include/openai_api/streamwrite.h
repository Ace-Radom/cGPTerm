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

#ifdef READ_CURSOR_DELAY
#define RCD READ_CURSOR_DELAY
#else
#define RCD 5000
#endif

extern char* stream_response_msg_only_buf;
extern size_t print_begin_cursor_x;
extern size_t print_begin_cursor_y;

void write_stream( const char* __msg );

#ifdef __cplusplus
}
#endif

#endif