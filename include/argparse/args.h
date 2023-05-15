#ifndef _ARGS_H_
#define _ARGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>

#include<argtable2.h>

#define ARG_PRINT_GLOSSARY_FORMAT "  %-30s %s\n"

// --------------------------- normal args ---------------------------

extern struct arg_lit* arg_help;
extern struct arg_lit* arg_raw;
extern struct arg_str* arg_load;

// ------------------------- set config args -------------------------

extern struct arg_str* arg_set_host;
extern struct arg_str* arg_set_apikey;
extern struct arg_dbl* arg_set_timeout;
extern struct arg_lit* arg_set_gentitle;
extern struct arg_str* arg_set_saveperfix;
extern struct arg_str* arg_set_loglevel;

// ---------------------------- argtable2 ----------------------------

extern struct arg_end* end_arg;
extern void* argtable[];

// -------------------------- args function --------------------------

void args_init();
int args_parse( int __argc , char** __argv );
void args_free();

#ifdef __cplusplus
}
#endif

#endif