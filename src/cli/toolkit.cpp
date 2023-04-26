#include"cli/toolkit.hpp"

struct termios ori_attr;

const std::string wait_char[] = { ".  " , ".. " , "..." , " .." , "  ." , "   " };

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

void print_wait_msg( std::string __msg ){
    static int counter = 0;
    std::cout << __msg << wait_char[counter%6] << '\r' << std::flush;
    counter++;
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
    return;
}