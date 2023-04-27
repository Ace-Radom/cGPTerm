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

/**
 * @brief erase space at begin and end of a str
*/
char* trim( char* __str ){
    int i = 0;
    int j = strlen( __str ) - 1;
    while ( __str[i] == ' ' )
        ++i;
    while ( __str[j] == ' ' )
        --j;
    if ( i > j )
        return NULL;
    // here: i > j means there are only spaces in this str
    // if continue, it will lead to segmentation fault, therefore return NULL here
    char* newstr = ( char* ) malloc( strlen( __str ) );
    strncpy( newstr , __str + i , j - i + 1 );
    newstr[j-i+1] = '\0';
    return newstr;
}