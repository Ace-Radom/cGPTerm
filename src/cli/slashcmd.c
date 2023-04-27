#include"cli/slashcmd.h"

const char* slash_commands[] = {
    "/tokens",
    "/timeout",
    "/help",
    "/exit"
};

/**
 * @brief handle slash command
 * 
 * @param __slashcmd the slash command input
 * 
 * @return 0 when slash command successfully handled; 1 when unrecognized slash command occured; -1 when /exit triggered
*/
int handle_slash_command( const char* __slashcmd ){
    if ( strcmp( __slashcmd , "/tokens" ) == 0 )
    {
        ezylog_logdebug( logger , "/tokens command triggered" );
        printf( "Total Tokens Spent: %ld\n" , openai -> total_tokens_spent );
        printf( "Current Tokens: %d/%d\n" , openai -> current_tokens , openai -> tokens_limit );
        return 0;
    } // /tokens

    if ( strncmp( __slashcmd , "/timeout" , 8 ) == 0 )
    {
        ezylog_logdebug( logger , "/timeout command triggered" );
        double new_timeout;
        char* new_timeout_str;

        if ( strlen( __slashcmd ) == 8 )
            goto ask_timeout;
            // only input "/timeout", goto ask timeout length
        
        char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
        strcpy( temp , __slashcmd );
        char* token = strtok( temp , " " );
        token = strtok( NULL , " " );
        // get the second part str
        new_timeout = strtod( token , NULL );
        if ( new_timeout != 0 )
        {
            if ( new_timeout > 0 )
            {
                OPENAI_API_TIMEOUT = new_timeout;
                printf( "API timeout set to %.2lfs\n" , OPENAI_API_TIMEOUT );
                ezylog_loginfo( logger , "API timeout set to %lfs" , OPENAI_API_TIMEOUT );
                free( temp );
                return 0;
            }
            else
            {
                printf( "API timeout cannot be less than 0s\n" );
            } // illegal input
        }
        else
        {
            if ( new_timeout == 0 && token[0] == '0' )
                printf( "API timeout cannot be 0s\n" );
            else
                printf( "API timeout must be an integer or a float\n" );
        } // illegal input
        free( temp );

    ask_timeout:
        new_timeout_str = readline( "Please input new API timeout: " );
        new_timeout = strtod( new_timeout_str , NULL );
        if ( new_timeout != 0 )
        {
            if ( new_timeout > 0 )
            {
                OPENAI_API_TIMEOUT = new_timeout;
                printf( "API timeout set to %.2lfs\n" , OPENAI_API_TIMEOUT );
                ezylog_loginfo( logger , "API timeout set to %lfs" , OPENAI_API_TIMEOUT );
                free( new_timeout_str );
                return 0;
            }
            else
            {
                printf( "API timeout cannot be less than 0s\n" );
            } // illegal input
        }
        else
        {
            if ( new_timeout == 0 && new_timeout_str[0] == '0' )
                printf( "API timeout cannot be 0s\n" );
            else
                printf( "API timeout must be an integer or a float\n" );
        } // illegal input
        goto ask_timeout;
    } // /timeout TIMEOUT

    if ( strcmp( __slashcmd , "/help" ) == 0 )
    {
        print_slash_command_help();
        return 0;
    } // /help
    if ( strcmp( __slashcmd , "/exit" ) == 0 )
    {
        return -1;
    } // /exit, ready to break
    return 1;
}

void print_slash_command_help(){
    printf( "Available commands:\n" );
    printf( "    /tokens\t\t\t- Show the total tokens spent and the tokens for the current conversation\n" );
    printf( "    /timeout [new_timeout]\t- Modify the api timeout\n" );
    printf( "    /help\t\t\t- Show this help message\n" );
    printf( "    /exit\t\t\t- Exit the application\n" );
    return;
}