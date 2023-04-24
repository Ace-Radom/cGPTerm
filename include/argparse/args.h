#ifndef _ARGS_H_
#define _ARGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include<argtable2.h>

#define ARG_PRINT_GLOSSARY_FORMAT "  %-30s %s\n"

// --------------------------- normal args ---------------------------

extern struct arg_lit* arg_help;

// ------------------------- set config args -------------------------

extern struct arg_str* arg_set_apikey;
extern struct arg_dbl* arg_set_timeout;
extern struct arg_lit* arg_set_gentitle;
extern struct arg_str* arg_set_saveprefix;
extern struct arg_str* arg_set_loglevel;

// ---------------------------- argtable2 ----------------------------

extern struct arg_end* end_arg;
extern void* argtable[];

// -------------------------- arg get value --------------------------

// gav stands for Get Arg Value

#define gav_help           arg_help           -> count > 0 ? true : false
#define gav_set_apikey     arg_set_apikey     -> sval[0]
#define gav_set_timeout    arg_set_timeout    -> dval[0]
#define gav_set_gentitle   arg_set_gentitle   -> count > 0 ? true : false
#define gav_set_saveprefix arg_set_saveprefix -> sval[0]
#define gav_set_loglevel   arg_set_loglevel   -> sval[0]

// -------------------------- args function --------------------------

void args_init();
int args_parse( int __argc , char** __argv );
void args_free();

#ifdef __cplusplus
}
#endif

#endif