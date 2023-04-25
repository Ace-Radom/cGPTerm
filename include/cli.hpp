#ifndef _CLI_HPP_
#define _CLI_HPP_

#ifdef __cplusplus

#include<iostream>
#include<string>

extern "C" {

#include"openai_api.h"
    
#endif

int start_CLI();

#ifdef __cplusplus
}
#endif

#endif