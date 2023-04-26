#ifndef _TOOLKIT_HPP_
#define _TOOLKIT_HPP_

#include<iostream>
#include<string>
#include<thread>
#include<chrono>
#include<termios.h>
#include<unistd.h>
#include<string.h>

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

extern struct termios ori_attr;

void get_original_terattr();
void reset_terattr();

void turn_off_echo();

void write_ANSI( const char* __ANSI );

void print_wait_msg( std::string __msg );

#endif