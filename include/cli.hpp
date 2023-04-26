#ifndef _CLI_HPP_
#define _CLI_HPP_

#ifdef __cplusplus

#include<iostream>
#include<string>
#include<thread>
#include"cli/toolkit.hpp"

extern "C" {

#include"openai_api.h"
    
#endif

int start_CLI();
// this is an api for main.c

#ifdef __cplusplus
}
#endif

#endif