#ifndef _CTIKTOKEN_HPP_
#define _CTIKTOKEN_HPP_

#ifdef __cplusplus
#include<iostream>
#include"encoding.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include<stdlib.h>

void cout_tokens_init();
size_t count_tokens( const char* __text );

#ifdef __cplusplus
}
#endif

#endif