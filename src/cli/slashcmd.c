#include"cli/slashcmd.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/tokens",
    "/save",
    "/timeout",
    "/model",
    "/help",
    "/exit",
    NULL
};

void disable_history_search( void ){
    rl_bind_keyseq( "\\e[A" , NULL ); // disable up arrow key
    rl_bind_keyseq( "\\e[B" , NULL ); // disable down arrow key
}

void enable_history_search( void ){
    rl_bind_keyseq( "\\e[A" , rl_history_search_backward ); // enable up arrow key
    rl_bind_keyseq( "\\e[B" , rl_history_search_forward );  // enable down arrow key
}

char* chat_history_save_file_generated_path = NULL;

/**
 * @brief this startup hook will automaticlly generate a file name for chat history save file
*/
void save_chat_history_startup_hook( void ){
    chat_history_save_file_generated_path = ( char* ) malloc( 256 );
    strcpy( chat_history_save_file_generated_path , CHAT_SAVE_PERFIX );
    // copy chat save perfix to here

    char* curtime = ( char* ) malloc( 40 );
    struct timeval tv;
    gettimeofday( &tv , NULL );
    time_t curtime_ts = tv.tv_sec;
    strftime( curtime , 40 , "%Y-%m-%d_%H,%M,%S" , localtime( &curtime_ts ) );
    // get current time str

    strcat( chat_history_save_file_generated_path , curtime );
    strcat( chat_history_save_file_generated_path , ".json" );
    // build file name

    rl_insert_text( chat_history_save_file_generated_path );
    
    free( curtime );
    rl_startup_hook = NULL;
    // reset hook
}

/**
 * @brief handle slash command
 * 
 * @param __slashcmd the slash command input
 * 
 * @return 0 when slash command successfully handled; 1 when unrecognized slash command occured; -1 when /exit triggered
*/
int handle_slash_command( const char* __slashcmd ){

// ===================================================================================
// ===================================== /tokens =====================================
// ===================================================================================

    if ( strcmp( __slashcmd , "/tokens" ) == 0 )
    {
        ezylog_logdebug( logger , "/tokens command triggered" );
        char* total_tokens_spent_str = ( char* ) malloc( 64 );
        char* current_tokens_str = ( char* ) malloc( 64 );
        sprintf( total_tokens_spent_str , "[bright magenta]Total Tokens Spent:[/] \t%ld" , openai -> total_tokens_spent );
        sprintf( current_tokens_str , "[green]Current Tokens:[/] \t%d/%d" , openai -> current_tokens , openai -> tokens_limit );
        crpanel( "token_summary" , 40 , "bold" , 2 , total_tokens_spent_str , current_tokens_str );
        // crprint( "[bright magenta]Total Tokens Spent:[/] %ld\n" , openai -> total_tokens_spent );
        // crprint( "[green]Current Tokens:[/] %d/%d[/]\n" , openai -> current_tokens , openai -> tokens_limit );
        free( total_tokens_spent_str );
        free( current_tokens_str );
        return 0;
    } // /tokens

// =================================================================================
// ===================================== /save =====================================
// =================================================================================

    if ( strncmp( __slashcmd , "/save" , 5 ) == 0 )
    {
        ezylog_logdebug( logger , "/save command triggered" );
        char* save_path;

        if ( strlen( __slashcmd ) == 5 )
        {
            disable_history_search();
            rl_startup_hook = save_chat_history_startup_hook;
            // print default (generated) save file path
            save_path = readline( "Save to: " );
            enable_history_search();
        } // only input '/save', goto generate filename and save
        else
        {
            char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
            strcpy( temp , __slashcmd );
            char* token = strtok( temp , " " );
            token = strtok( NULL , " " );
            // get the second part str
            save_path = ( char* ) malloc( strlen( token ) + 1 );
            strcpy( save_path , token );
        } // save path given

        FILE* savef = fopen( save_path , "w" );
        if ( savef == NULL )
        {
            crprint( "[red]Save chat history to '[bold]%s[/]' failed, check log for more informations\n" , save_path );
            char* errmsg = strerror( errno );
            // get error message
            ezylog_logerror( logger , "Save chat history to '%s' failed: errno %d, error message \"%s\"" , save_path , errno , errmsg );
            return 0;
        } // open save file failed; log error message

        int rc = openai_save_history( savef );
        if ( rc == -1 )
        {
            crprint( "[red]Save chat history to '[bold]%s[/]' failed due to unknown error\n" , save_path );
            ezylog_logerror( logger , "Save chat history to '%s' failed due to unknown error" , save_path );
        } // dump json failed
        else
        {
            crprint( "[dim]Chat history saved to: [green]%s[/]\n" , save_path );
            ezylog_loginfo( logger , "Chat history saved to: '%s'" , save_path );
        }
        fclose( savef );
        if ( chat_history_save_file_generated_path != NULL )
            free( chat_history_save_file_generated_path );
        free( save_path );
        return 0;
    }

// ====================================================================================
// ===================================== /timeout =====================================
// ====================================================================================

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

// ==================================================================================
// ===================================== /model =====================================
// ==================================================================================

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

// ========================================================================================
// ===================================== /help, /exit =====================================
// ========================================================================================

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
    crprint( "    [bright magenta]/save[/] [bold]\\[filename_or_path][/]\t- Save the chat history to a file, suggest title if filename_or_path not provided\n" );
    crprint( "    [bright magenta]/timeout[/] [bold]\\[new_timeout][/]\t- Modify the api timeout\n" );
    crprint( "    [bright magenta]/model[/] [bold]\\[model_name][/]\t\t- Change AI model\n" );
    crprint( "    [bright magenta]/help[/]\t\t\t- Show this help message\n" );
    crprint( "    [bright magenta]/exit[/]\t\t\t- Exit the application\n" );
    return;
}