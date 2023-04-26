#ifndef _TOOLKIT_H_
#define _TOOLKIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<termios.h>

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

extern struct termios ori_attr;

void get_original_terattr();
void reset_terattr();

void turn_off_echo();

void write_ANSI( const char* __ANSI );

void print_wait_msg( const char* __msg );

#ifdef __cplusplus
}
#endif

#endif