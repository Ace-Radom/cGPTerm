#ifndef _CLI_H_
#define _CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<pthread.h>
#include<readline/readline.h>
#include<readline/history.h>

#include"openai_api.h"
#include"cli/toolkit.h"

int start_CLI();
// this is an api for main.c

#ifdef __cplusplus
}
#endif

#endif