#ifndef _SLASHCMD_H_
#define _SLASHCMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<sys/time.h>

#include<readline/readline.h>
#include<readline/history.h>

#include"openai_api.h"
#include"ezylog.h"
#include"utils.h"
#include"crich.h"

extern const char* slash_commands[];
extern char* chat_history_save_file_generated_path;

int handle_slash_command( const char* __slashcmd );

void print_slash_command_help();

#ifdef __cplusplus
}
#endif

#endif