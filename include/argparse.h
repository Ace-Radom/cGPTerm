#ifndef _ARGPARSE_H_
#define _ARGPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"rwcfg.h"
#include"argparse/args.h"
#include"argparse/gav.h"
#include"utils.h"

int args_actions_setcfg();
void print_help();

#ifdef __cplusplus
}
#endif

#endif