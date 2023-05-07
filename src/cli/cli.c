#include"cli.h"

bool match_exit_word( const char* __str );

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    get_original_terattr();

    signal( SIGINT , SIGINT_handler );

    rl_attempted_completion_function = rl_attempted_completion_callback;
    rl_completion_display_matches_hook = rl_completion_display_matches_hook_callback;

    while ( 1 )
    {
        char* input;
        input = readline( "> " );
        printf( "\033[2K\r" );
        // clear possible output
        if ( input == NULL )
            break;
        // EOF raised, break
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
        {
            crstatus( "[bold][bright cyan]ChatGPT is thinking...\r" , "green" );
            usleep( 100000 );
        }
        // until request done: print wait msg
        printf( "\r\033[2K\r" );
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
                md_set( data.response );
                md_parse();
                md_print();
                printf( "\n" );
                if ( openai -> tokens_limit - openai -> current_tokens < 500 && openai -> tokens_limit - openai -> current_tokens >= 1 )
                    crprint( "[dim]Approaching tokens limit: %d tokens left\n" , openai -> tokens_limit - openai -> current_tokens );
                if ( openai -> tokens_limit - openai -> current_tokens < 1 )
                    crprint( "[red]Reached tokens limit: %d\n" , openai -> tokens_limit );
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

        if ( curl_request_abort_called )
        {
            crprint( "[bold][bright cyan]Aborted\n" );
            curl_request_abort_called = false;
        } // request abort raised, print abort msg and reset this signal

        if ( match_exit_word( input_trim ) )
        {
            free( input_trim );
            break;
        } // match exit word

        free( input_trim );
    }
    return 0;
}

bool match_exit_word( const char* __str ){
    char* str = ( char* ) malloc( strlen( __str ) + 1 );
    strcpy( str , __str );
    char* stralllower = str;
    for ( ; *str != '\0' ; str++ )
        *str = tolower( *str );
    if ( strcmp( stralllower , "再见" ) == 0 ||
         strcmp( stralllower , "bye" ) == 0 ||
         strcmp( stralllower , "goodbye" ) == 0 ||
         strcmp( stralllower , "结束" ) == 0 ||
         strcmp( stralllower , "end" ) == 0 ||
         strcmp( stralllower , "退出" ) == 0 ||
         strcmp( stralllower , "exit" ) == 0 ||
         strcmp( stralllower , "quit" ) == 0 )
    {
        free( stralllower );
        return true;
    }
    free( stralllower );
    return false;
}