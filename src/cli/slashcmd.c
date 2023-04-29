#include"cli/slashcmd.h"

const char* slash_commands[] = {
    "/tokens",
    "/timeout",
    "/model",
    "/help",
    "/exit",
    NULL
};

void disable_history_search(void){
    rl_bind_keyseq( "\\e[A" , NULL ); // disable up arrow key
    rl_bind_keyseq( "\\e[B" , NULL ); // disable down arrow key
}

void enable_history_search( void ){
    rl_bind_keyseq( "\\e[A" , rl_history_search_backward ); // enable up arrow key
    rl_bind_keyseq( "\\e[B" , rl_history_search_forward );  // enable down arrow key
}

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
        crprint( "[bright magenta]Total Tokens Spent:[/] %ld\n" , openai -> total_tokens_spent );
        crprint( "[green]Current Tokens:[/] %d/[bold]%d[/]\n" , openai -> current_tokens , openai -> tokens_limit );
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
                crprint( "[dim]API timeout set to [green]%.2lfs[/].\n" , OPENAI_API_TIMEOUT );
                ezylog_loginfo( logger , "API timeout set to %lfs" , OPENAI_API_TIMEOUT );
                free( temp );
                return 0;
            }
            else
            {
                crprint( "\n[red]API timeout cannot be less than 0s\n\r" );
                // here: print error msg in the second line, then back to the first line and print "Please input new API timeout"
            } // illegal input
        }
        else
        {
            if ( new_timeout == 0 && token[0] == '0' )
                crprint( "\n[red]API timeout cannot be less than 0s\033[1A\r" );
            else
                crprint( "\n[red]API timeout must be an integer or a float\033[1A\r" );
            // here: print error msg in the second line, then back to the first line and print "Please input new API timeout"
        } // illegal input
        free( temp );

    ask_timeout:
        disable_history_search();
        new_timeout_str = readline( "Please input new API timeout: " );
        enable_history_search();
        new_timeout = strtod( new_timeout_str , NULL );
        if ( new_timeout != 0 )
        {
            if ( new_timeout > 0 )
            {
                OPENAI_API_TIMEOUT = new_timeout;
                printf( "\r\033[2K\r" );
                crprint( "[dim]API timeout set to [green]%.2lfs[/].\n" , OPENAI_API_TIMEOUT );
                ezylog_loginfo( logger , "API timeout set to %lfs" , OPENAI_API_TIMEOUT );
                free( new_timeout_str );
                return 0;
            }
            else
            {
                printf( "\r\033[2K\r" );
                crprint( "[red]API timeout cannot be less than 0s\n" );
            } // illegal input
        }
        else
        {
            if ( new_timeout == 0 && new_timeout_str[0] == '0' )
            {
                printf( "\r\033[2K\r" );
                crprint( "[red]API timeout cannot be less than 0s\n" );
            }
            else
            {
                printf( "\r\033[2K\r" );
                crprint( "[red]API timeout must be an integer or a float\n" );
            }
        } // illegal input
        printf( "\033[2A\r\033[2K\r" );
        fflush( stdout );
        // clear last "Please input new API timeout" output
        // also: before all error output here old error output need to be cleaned
        goto ask_timeout;
    } // /timeout TIMEOUT

    if ( strncmp( __slashcmd , "/model" , 6 ) == 0 )
    {
        ezylog_logdebug( logger , "/model command triggered" );
        char* new_model;

        if ( strlen( __slashcmd ) == 6 )
        {
            crprint( "\n[green]Model list: '[bold]gpt-3.5-turbo[/]', '[bold]gpt-4[/]', '[bold]gpt-4-32k[/]'\033[1A\r" );
            goto ask_model;
        }
        // only input "/model", goto ask new model

        char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
        strcpy( temp , __slashcmd );
        char* token = strtok( temp , " " );
        token = strtok( NULL , " " );
        // get the second part str
        new_model = token;
        if ( openai_set_model( new_model ) == 0 )
        {
            crprint( "[dim]Model has been set to [green]'%s'[/].\n" , openai -> model );
            ezylog_loginfo( logger , "Model has been set to '%s'" , openai -> model );
            free( temp );
            return 0;
        } // set successfully
        else
            crprint( "\n[red]Model can only be '[bold]gpt-3.5-turbo[/]', '[bold]gpt-4[/]', '[bold]gpt-4-32k[/]'\033[1A\r" );
        // illegal model

        free( temp );

    ask_model:
        disable_history_search();
        new_model = readline( "Please input new Model: " );
        enable_history_search();
        // disable history view when asking new timeout
        if ( openai_set_model( new_model ) == 0 )
        {
            printf( "\r\033[2K\r" );
            crprint( "[dim]Model has been set to [green]'%s'[/].\n" , openai -> model );
            ezylog_loginfo( logger , "Model has been set to '%s'" , openai -> model );
            free( new_model );
            return 0;
        } // set successfully
        else
        {
            printf( "\r\033[2K\r" );
            crprint( "[red]Model can only be '[bold]gpt-3.5-turbo[/]', '[bold]gpt-4[/]', '[bold]gpt-4-32k[/]'\n" );
        } // illegal model
        printf( "\033[2A\r\033[2K\r" );
        fflush( stdout );
        // clear last "Please input new Model" output
        goto ask_model;
    } // /model MODEL

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
    crprint( "[bold]Available commands:\n" );
    crprint( "    [bright magenta]/tokens[/]\t\t\t- Show the total tokens spent and the tokens for the current conversation\n" );
    crprint( "    [bright magenta]/timeout[/] [bold]\\[new_timeout][/]\t- Modify the api timeout\n" );
    crprint( "    [bright magenta]/model[/] [bold]\\[model_name][/]\t\t- Change AI model\n" );
    crprint( "    [bright magenta]/help[/]\t\t\t- Show this help message\n" );
    crprint( "    [bright magenta]/exit[/]\t\t\t- Exit the application\n" );
    return;
}