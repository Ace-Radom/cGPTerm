#include"cli.h"

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    get_original_terattr();

    while ( 1 )
    {
        char* input;
        printf( "> " );
        input = readline( NULL );
        if ( !input || *input == '\0' )
            continue;
        if ( strcmp( input , "quit" ) == 0 )
            break;
        
        turn_off_echo();
        write_ANSI( HIDE_CURSOR );
        // turn off echo; hide cursor
        openai_datatransfer_t data;
        data.msg = ( char* ) malloc( strlen( input ) + 1 );
        strcpy( data.msg , input );
        data.response = NULL;
        // build transfer data
        pthread_t send_request;
        int ptrc = pthread_create( &send_request , NULL , openai_send_chatrequest , ( void* ) &data ); // pthread return code
        usleep( 10000 );
        // start request; wait 10 ms in order to let openai_send_chatrequest to lock request_working (-> true)
        while ( request_working )
        {
            print_wait_msg( "ChatGPT is thinking" );
        } // until request done: print wait msg
        printf( "\r                             \r" );
        fflush( stdout );
        pthread_join( send_request , NULL );
        // clean wait msg, request thread join
        reset_terattr();
        write_ANSI( SHOW_CURSOR );
        // reset attr; show cursor
        if ( data.response )
        {
            if ( HTTP_Response_code / 100 != 4 )
                printf( "ChatGPT:\n%s\n" , data.response );
            else
            {
                printf( "Request Error: %s\n" , data.response );
                openai_msg_popback();
            } // request error, pop last user's msg
        }
        else
            openai_msg_popback();
        // same: request error, pop last user's msg
    }
    return 0;
}