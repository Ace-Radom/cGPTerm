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

#include<readline/readline.h>

#include"sclist.h"
#include"crich.h"
#include"openai_api.h"

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

extern struct termios ori_attr;

void SIGINT_handler( int signum ); 

// ===============================================
// ================== CLI Tools ==================
// ===============================================

void get_original_terattr();
void reset_terattr();
void turn_off_echo();

void write_ANSI( const char* __ANSI );

char** rl_attempted_completion_callback( const char* text , int start , int end );
char* rl_completion_search( const char* text , int state );
void rl_completion_display_matches_hook_callback( char** matches , int num_matches , int max_length );

// ===============================================
// ================== str Tools ==================
// ===============================================

char* trim( char* __str );

#ifdef __cplusplus
}
#endif

#endif