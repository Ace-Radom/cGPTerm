#include"cli.h"

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    get_original_terattr();

    rl_attempted_completion_function = rl_attempted_completion_callback;

    while ( 1 )
    {
        char* input;
        input = readline( "> " );
        char* input_trim = trim( input );
        if ( !input_trim || *input_trim == '\0' )
            continue;
        if ( input_trim[0] == '/' )
        {
            int hscrc = handle_slash_command( input_trim ); // handle slash command return code
            add_history( input_trim );
            free( input_trim );
            if ( hscrc == -1 )
                break;
            // /exit input, break    
            continue;
        }
        // input parts

        add_history( input_trim );
        // add input to history list
        
        turn_off_echo();
        write_ANSI( HIDE_CURSOR );
        // turn off echo; hide cursor
        openai_datatransfer_t data;
        data.msg = ( char* ) malloc( strlen( input_trim ) + 1 );
        strcpy( data.msg , input_trim );
        data.response = NULL;
        // build transfer data
        pthread_t send_request;
        int ptrc = pthread_create( &send_request , NULL , openai_send_chatrequest , ( void* ) &data ); // pthread return code
        usleep( 10000 );
        // start request; wait 10 ms in order to let openai_send_chatrequest to lock request_working (-> true)
        while ( request_working )
            print_wait_msg( "ChatGPT is thinking" );
        // until request done: print wait msg
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
            {
                crprint( "[bold][bright cyan]ChatGPT:\n" );
                printf( "%s\n" , data.response );
            }
            else
            {
                crprint( "[bold][red]Request Error: " );
                printf( "%s\n" , data.response );
                openai_msg_popback();
            } // request error, pop last user's msg
        }
        else
            openai_msg_popback();
        // same: request error, pop last user's msg
    }
    return 0;
}