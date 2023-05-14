#include"openai_api/streamwrite.h"

char* stream_response_msg_only_buf = NULL;

size_t trow;                 // terminal row
size_t tcol;                 // terminal col
size_t print_begin_cursor_x;
size_t print_begin_cursor_y;

int get_cursor_position( size_t* x , size_t* y ){
    fd_set readset;
    int success = 0;
    struct timeval time;
    struct termios term , initial_term;

    tcgetattr( STDIN_FILENO , &initial_term );
    term = initial_term;
    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ECHO;
    tcsetattr( STDIN_FILENO , TCSANOW , &term );

    printf( "\33[6n" );
    fflush( stdout );
    // request position

    FD_ZERO( &readset );
    FD_SET( STDIN_FILENO , &readset );
    time.tv_sec = 0;
    time.tv_usec = 5000;
    // wait 20ms for a terminal answer

    if ( select( STDIN_FILENO + 1 , &readset , NULL , NULL , &time ) == 1 )
        if ( scanf( "\033[%ld;%ldR" , y , x ) == 2 )
            success = 1;
    // If it success, try to read the cursor value

    tcsetattr( STDIN_FILENO , TCSADRAIN , &initial_term );
    // set back the properties of the terminal

    return success;
}

/**
 * @brief get terminal info (trow, tcol, cursor pos)
*/
void get_tinfo(){
    struct winsize size;
    ioctl( STDOUT_FILENO , TIOCGWINSZ , &size );
    trow = size.ws_row;
    tcol = size.ws_col;
    // get terminal size

    get_cursor_position( &print_begin_cursor_x , &print_begin_cursor_y );
    return;
}

void write_stream( const char* __msg ){
    if ( strlen( stream_response_msg_only_buf ) == 0 )
    {
        crprint( "[bold][bright cyan]ChatGPT:\n" );
        get_tinfo();
        md_set( NULL );
    }
    
    if ( raw_mode_enable )
    {
        printf( "%s" , __msg );
        fflush( stdout );
    } // raw mode
    else
    {
        printf( "\033[%ld;%ldH\033[2K\033[J\r" , print_begin_cursor_y , print_begin_cursor_x );
        fflush( stdout );
        // clean output before
        md_set( stream_response_msg_only_buf );
        md_parse();
        md_print();
        fflush( stdout );
    }

    strcat( stream_response_msg_only_buf , __msg );
}