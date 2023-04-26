#include"cli/toolkit.h"

struct termios ori_attr;

const char* wait_char[] = { ".  " , ".. " , "..." , " .." , "  ." , "   " };

void get_original_terattr(){
    tcgetattr( STDIN_FILENO , &ori_attr );
    return;
}

void reset_terattr(){
    tcsetattr( STDIN_FILENO , TCSAFLUSH , &ori_attr );
    return;
}

void turn_off_echo(){
    struct termios new_attr = ori_attr;
    new_attr.c_lflag &= ~( ECHO | ICANON );
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO , TCSAFLUSH , &new_attr );
    return;
}

void write_ANSI( const char* __ANSI ){
    write( STDIN_FILENO , __ANSI , strlen( __ANSI ) );
    return;
}

void print_wait_msg( const char* __msg ){
    static int counter = 0;
    printf( "%s%s\r" , __msg , wait_char[counter] );
    fflush( stdout );
    counter == 5 ? counter = 0 
                 : counter++;
    usleep( 100000 );
    // sleep 100 ms
    return;
}