#ifndef _GAV_H_
#define _GAV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdbool.h>

#include"args.h"

// gota stands for Got Arg

#define gota_help           arg_help           -> count > 0 ? true : false
#define gota_raw            arg_raw            -> count > 0 ? true : false
#define gota_load           arg_load           -> count > 0 ? true : false
#define gota_set_host       arg_set_host       -> count > 0 ? true : false
#define gota_set_apikey     arg_set_apikey     -> count > 0 ? true : false
#define gota_set_timeout    arg_set_timeout    -> count > 0 ? true : false
#define gota_set_gentitle   arg_set_gentitle   -> count > 0 ? true : false
#define gota_set_saveperfix arg_set_saveperfix -> count > 0 ? true : false
#define gota_set_loglevel   arg_set_loglevel   -> count > 0 ? true : false

// gav stands for Get Arg Value

#define gav_help           gota_help
#define gav_raw            gota_raw
#define gav_load           arg_load           -> sval[0]
#define gav_set_host       arg_set_host       -> sval[0]
#define gav_set_apikey     arg_set_apikey     -> sval[0]
#define gav_set_timeout    arg_set_timeout    -> dval[0]
#define gav_set_gentitle   gota_set_gentitle
#define gav_set_saveperfix arg_set_saveperfix -> sval[0]
#define gav_set_loglevel   arg_set_loglevel   -> sval[0]

#ifdef __cplusplus
}
#endif

#endif