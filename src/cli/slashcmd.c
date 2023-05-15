#include"cli/slashcmd.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/raw",
    "/stream",
    "/title",
    "/tokens",
    "/usage",
    "/timeout",
    "/model",
    "/system",
    "/rand",
    "/save",
    "/undo",
    "/delete",
    "/last",
    "/copy",
    "/version",
    "/list",
    "/help",
    "/exit",
    NULL
};

#define maxn( x , y ) ( x > y ? x : y )
#define minn( x , y ) ( x < y ? x : y )

void disable_history_search( void ){
    rl_bind_keyseq( "\\e[A" , NULL ); // disable up arrow key
    rl_bind_keyseq( "\\e[B" , NULL ); // disable down arrow key
}

void enable_history_search( void ){
    rl_bind_keyseq( "\\e[A" , rl_history_search_backward ); // enable up arrow key
    rl_bind_keyseq( "\\e[B" , rl_history_search_forward );  // enable down arrow key
}

char* chat_history_save_file_generated_path = NULL;

void search_codes( c_vector* __codelist , const char* __text ){
    char* start;
    char* end;
    start = strstr( __text , "```" );
    while ( start != NULL )
    {
        end = strstr( start + 3 , "```" );
        if ( end == NULL )
            break;
        char* this_code = ( char* ) malloc( ( end - start + 8 ) * sizeof( char ) );
        strncpy( this_code , start , end - start + 3 );
        this_code[end-start+3] = '\0';
        cv_push_back( __codelist , ( void* ) this_code );
        start = strstr( end + 3 , "```" );
    }
    return;
}

double get_jaccard_similarity( const char* __s1 , const char* __s2 ){
    size_t s1_len = strlen( __s1 );
    size_t s2_len = strlen( __s2 );
    char* unionSet = ( char* ) malloc( s1_len + s2_len + 1 );
    char* intersectionSet = ( char* ) malloc( maxn( s1_len , s2_len ) + 1 );
    int counter = 0;

    strcpy( unionSet , __s1 );
    counter = strlen( unionSet );
    for ( size_t i = 0 ; i < s2_len ; i++ )
    {
        bool in_union = false;
        for ( size_t j = 0 ; j < s1_len ; j++ )
            if ( __s2[i] == __s1[j] )
            {
                in_union = true;
                break;
            }
        if ( !in_union )
            // strcat( unionSet , &__s2[i] );
            unionSet[counter++] = __s2[i];
    }
    unionSet[counter] = '\0';
    // union set

    counter = 0;
    for ( size_t i = 0 ; i < s1_len ; i++ )
        for ( size_t j = 0 ; j < s2_len ; j++ )
            if ( __s1[i] == __s2[j] )
            {
                intersectionSet[counter++] = __s1[i];
                break;
            }
    intersectionSet[counter] = '\0';
    // intersection set

    return ( double ) strlen( intersectionSet ) / strlen( unionSet );
}

int get_levenshtein_distance( const char* __s1 , const char* __s2 ){
    int s1_len = strlen( __s1 );
    int s2_len = strlen( __s2 );

    int v[s1_len+1][s2_len+1];
    memset( v , 0 , sizeof( v ) );
    for ( int i = 0 ; i <= s1_len ; i++ )
    {
        for ( int j = 0 ; j <= s2_len ; j++ )
        {
            if ( i == 0 )
                v[i][j] = j;
            else if ( j == 0 )
                v[i][j] = i;
            else if ( __s1[i-1] == __s2[j-1] )
                v[i][j] = v[i-1][j-1];
            else
                v[i][j] = minn( v[i-1][j-1] , minn( v[i][j-1] , v[i-1][j] ) ) + 1;
        }
    }
    return v[s1_len][s2_len];
}

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

    char* slashcmd_headcmd_only_temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
    strcpy( slashcmd_headcmd_only_temp , __slashcmd );
    char* slashcmd_headcmd_only = strtok( slashcmd_headcmd_only_temp , " " );

// ================================================================================
// ===================================== /raw =====================================
// ================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/raw" ) == 0 )
    {
        ezylog_logdebug( logger , "/raw command triggered" );
        raw_mode_enable = !raw_mode_enable;
        if ( raw_mode_enable )
            crprint( "[dim]Raw mode enabled, use `[bright magenta]/last[/]` to display the last answer.\n" );
        else
            crprint( "[dim]Raw mode disabled, use `[bright magenta]/last[/]` to display the last answer.\n" );

        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /raw

// ===================================================================================
// ===================================== /stream =====================================
// ===================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/stream" ) == 0 )
    {
        ezylog_logdebug( logger , "/stream command triggered" );
        openai -> stream_mode = !openai -> stream_mode;
        if ( openai -> stream_mode )
        {
            ezylog_loginfo( logger , "Stream mode enabled" );
            crprint( "[dim]Stream mode enabled, the answer will start outputting as soon as the first response arrives.\n" );
        }
        else
        {
            ezylog_loginfo( logger , "Stream mode disabled" );
            crprint( "[dim]Stream mode disabled, the answer is being displayed after the server finishes responding.\n" );
        }
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /stream

// ==================================================================================
// ===================================== /title =====================================
// ==================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/title" ) == 0 )
    {
        ezylog_logdebug( logger , "/title command triggered" );
        AUTO_GENERATE_TITLE = !AUTO_GENERATE_TITLE;
        if ( AUTO_GENERATE_TITLE )
        {
            ezylog_loginfo( logger , "Auto title generation enabled" );
            crprint( "[dim]Auto title generation enabled." );
            if ( openai_get_message_list_length() >= 3 && openai -> title == NULL )
            {
                crprint( "[dim] Generate a new title for current chat now...\n" );
                
                pthread_t generate_title_background;
                pthread_attr_t generate_title_background_attr;
                pthread_attr_init( &generate_title_background_attr );
                pthread_attr_setdetachstate( &generate_title_background_attr , PTHREAD_CREATE_DETACHED );
                ezylog_logdebug( logger , "title background generation triggered, call generate function" );
                pthread_create( &generate_title_background , &generate_title_background_attr , openai_generate_title , ( void* ) openai_getfirst() );
                pthread_attr_destroy( &generate_title_background_attr );
            } // current chat not empty, generate one title now
            else
                printf( "\n" );
        }
        else
        {
            ezylog_loginfo( logger , "Auto title generation disabled" );
            crprint( "[dim]Auto title generation disabled.\n" );
        }
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /title

// ===================================================================================
// ===================================== /tokens =====================================
// ===================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/tokens" ) == 0 )
    {
        ezylog_logdebug( logger , "/tokens command triggered" );
        char* total_tokens_spent_str = ( char* ) malloc( 64 );
        char* current_tokens_str = ( char* ) malloc( 64 );
        sprintf( total_tokens_spent_str , "[bright magenta]Total Tokens Spent:[/] \t%ld" , openai -> total_tokens_spent );
        sprintf( current_tokens_str , "[green]Current Tokens:[/] \t%d/[bold]%d[/]" , openai -> current_tokens , openai -> tokens_limit );
        crpanel( "token_summary" , NULL , 40 , NULL , 2 , total_tokens_spent_str , current_tokens_str );
        free( total_tokens_spent_str );
        free( current_tokens_str );
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /tokens

// =================================================================================
// ===================================== /save =====================================
// =================================================================================

    if ( strncmp( slashcmd_headcmd_only , "/save" , 5 ) == 0 )
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
            free( slashcmd_headcmd_only_temp );
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
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /save FILE

// ====================================================================================
// ===================================== /timeout =====================================
// ====================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/timeout" ) == 0 )
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
                free( slashcmd_headcmd_only_temp );
                return 0;
            }
            else
            {
                crprint( "\n[red]API timeout cannot be less than 0s\033[1A\r" );
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
                free( slashcmd_headcmd_only_temp );
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

    if ( strcmp( slashcmd_headcmd_only , "/model" ) == 0 )
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
            free( slashcmd_headcmd_only_temp );
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
            free( slashcmd_headcmd_only_temp );
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

// ===================================================================================
// ===================================== /system =====================================
// ===================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/system" ) == 0 )
    {
        ezylog_logdebug( logger , "/system command triggered" );
        char* new_prompt;

        if ( strlen( __slashcmd ) == 7 )
        {
            disable_history_search();
            new_prompt = readline( "Please input new system prompt: " );
            enable_history_search();
        }
        else
        {
            char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
            strcpy( temp , __slashcmd );
            char* token = strtok( temp , " " );
            token = strtok( NULL , " " );
            // get the second part str
            new_prompt = ( char* ) malloc( strlen( token ) + 1 );
            strcpy( new_prompt , token );
            free( temp );
        }
        openai_set_prompt( new_prompt );
        crprint( "[dim]System prompt has been modified to '%s'\n" , new_prompt );
        ezylog_loginfo( logger , "System prompt has been modified to '%s'" , new_prompt );
        free( new_prompt );
        free( slashcmd_headcmd_only_temp );
        return 0;
    }

// =================================================================================
// ===================================== /rand =====================================
// =================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/rand" ) == 0 )
    {
        ezylog_logdebug( logger , "/rand command triggered" );
        double new_temperature;
        char* new_temperature_str;

        if ( strlen( __slashcmd ) == 5 )
            goto ask_temperature;
        // only input "/rand", goto ask temperature

        char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
        strcpy( temp , __slashcmd );
        char* token = strtok( temp , " " );
        token = strtok( NULL , " " );
        // get the second part str
        new_temperature = strtod( token , NULL );
        if ( new_temperature != 0 )
        {
            if ( new_temperature > 0 && new_temperature <= 2 )
            {
                openai_set_temperature( new_temperature );
                crprint( "[dim]Randomness set to [green]%.2lf[/].\n" , openai -> temperature );
                ezylog_loginfo( logger , "Randomness set to %lf" , openai -> temperature );
                free( temp );
                free( slashcmd_headcmd_only_temp );
                return 0;
            }
            else
            {
                crprint( "\n[red]Randomness must be a real number in range of 0 and 2\033[1A\r" );
            } // illegal input
        }
        else
        {
            if ( new_temperature == 0 && token[0] == '0' )
            {
                openai_set_temperature( new_temperature );
                crprint( "[dim]Randomness set to [green]%.2lf[/].\n" , openai -> temperature );
                ezylog_loginfo( logger , "Randomness set to %lf" , openai -> temperature );
                free( temp );
                free( slashcmd_headcmd_only_temp );
                return 0;
            } // temperature is 0
            else
            {
                crprint( "\n[red]Randomness must be a real number in range of 0 and 2\033[1A\r" );
            } // illegal input
        }
        free( temp );

    ask_temperature:
        disable_history_search();
        new_temperature_str = readline( "Please input new Randomness: " );
        enable_history_search();
        new_temperature = strtod( new_temperature_str , NULL );
        if ( new_temperature != 0 )
        {
            if ( new_temperature > 0 && new_temperature <= 2 )
            {
                openai_set_temperature( new_temperature );
                printf( "\r\033[2K\r" );
                crprint( "[dim]Randomness set to [green]%.2lf[/].\n" , openai -> temperature );
                ezylog_loginfo( logger , "Randomness set to %lf" , openai -> temperature );
                free( new_temperature_str );
                free( slashcmd_headcmd_only_temp );
                return 0;
            }
            else
            {
                printf( "\r\033[2K\r" );
                crprint( "[red]Randomness must be a real number in range of 0 and 2\n" );
            } // illegal input
        }
        else
        {
            if ( new_temperature == 0 && new_temperature_str[0] == '0' )
            {
                openai_set_temperature( new_temperature );
                printf( "\r\033[2K\r" );
                crprint( "[dim]Randomness set to [green]%.2lf[/].\n" , openai -> temperature );
                ezylog_loginfo( logger , "Randomness set to %lf" , openai -> temperature );
                free( new_temperature_str );
                free( slashcmd_headcmd_only_temp );
                return 0;
            } // temperature is 0
            else
            {
                printf( "\r\033[2K\r" );
                crprint( "[red]Randomness must be a real number in range of 0 and 2\n" );
            } // illegal input
        }
        printf( "\033[2A\r\033[2K\r" );
        fflush( stdout );
        goto ask_temperature;
    } // /rand TEMPERATURE

// ==================================================================================
// ===================================== /usage =====================================
// ==================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/usage" ) == 0 )
    {
        ezylog_logdebug( logger , "/usage command triggered" );
        
        turn_off_echo();
        write_ANSI( HIDE_CURSOR );
        // turn off echo; hide cursor
        while ( request_working );
        // safety line: if still a request working, wait
        pthread_t get_usage_summary;
        pthread_create( &get_usage_summary , NULL , openai_get_usage_summary , NULL );
        usleep( 10000 );
        // start get usage request; wait 10 ms in order to let openai_get_usage to lock request_working (-> true)
        while ( request_working )
        {
            crstatus( "[bold][bright blue]Getting credit usage...\r" , "green" );
            usleep( 100000 );
        }
        // until request done: print wait msg
        printf( "\r\033[2K\r" );
        fflush( stdout );
        pthread_join( get_usage_summary , NULL );
        // clean wait msg, get usage request thread join
        reset_terattr();
        write_ANSI( SHOW_CURSOR );
        // reset attr; show cursor
        char* total_granted_str = ( char* ) malloc( 64 );
        char* used_this_month_str = ( char* ) malloc( 64 );
        char* used_total_str = ( char* ) malloc( 64 );
        char* plan_str = ( char* ) malloc( 64 );
        sprintf( total_granted_str , "[green]Total Granted:[/]\t$%.2lf" , openai -> credit_total_granted );
        sprintf( used_this_month_str , "[bright cyan]Used This Month:[/]\t$%.2lf" , openai -> credit_used_this_month );
        sprintf( used_total_str , "[bright blue]Used Total:[/]\t\t$%.2lf" , openai -> credit_total_used );
        sprintf( plan_str , "Plan: %s" , openai -> credit_plan );
        crpanel( "Credit Summary" , plan_str , 36 , NULL , 3 , total_granted_str , used_this_month_str , used_total_str );
        free( total_granted_str );
        free( used_this_month_str );
        free( used_total_str );
        free( plan_str );
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /usage

// =================================================================================
// ===================================== /undo =====================================
// =================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/undo" ) == 0 )
    {
        ezylog_logdebug( logger , "/undo command triggered" );
        openai_undo();
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /undo

// ===================================================================================
// ===================================== /delete =====================================
// ===================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/delete" ) == 0 )
    {
        ezylog_logdebug( logger , "/delete command triggered" );
        
        if ( strlen( __slashcmd ) == 7 )
        {
            openai_delete_first();
            free( slashcmd_headcmd_only_temp );
            return 0;
        } // no other args given, delete first

        char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
        char* second_arg;
        strcpy( temp , __slashcmd );
        second_arg = strtok( temp , " " );
        second_arg = strtok( NULL , " " );

        if ( strcmp( second_arg , "first" ) == 0 )
            openai_delete_first();
        // /delete first
        else if ( strcmp( second_arg , "all" ) == 0 )
            openai_delete_all();
        else
            crprint( "[dim]Nothing to do. Available copy command: `[bright magenta]/delete first[/]` or `[bright magenta]/delete all[/]`\n" );
        // /delete all
        free( temp );
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /delete

// =================================================================================
// ===================================== /last =====================================
// =================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/last" ) == 0 )
    {
        ezylog_logdebug( logger , "/last command triggered" );
        const char* last_response = openai_getlast();
        if ( !last_response )
        {
            crprint( "[dim]Nothing to print\n" );
            free( slashcmd_headcmd_only_temp );
            return 0;
        }
        crprint( "[bold][bright cyan]ChatGPT:\n" );
        if ( raw_mode_enable )
        {
            printf( "%s\n" , last_response );
        }
        else
        {
            md_set( last_response );
            md_parse();
            md_print( true );
            printf( "\n" );
        }
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /last

// =================================================================================
// ===================================== /copy =====================================
// =================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/copy" ) == 0 )
    {
        ezylog_logdebug( logger , "/copy command triggered" );
        const char* last_response = openai_getlast();
        if ( !last_response )
        {
            crprint( "[dim]Nothing to copy\n" );
            free( slashcmd_headcmd_only_temp );
            return 0;
        } // /copy command should not copy system prompt

        if ( strlen( __slashcmd ) == 5 )
        {
            clipboard_copy( last_response );
            crprint( "[dim]Last reply copied to Clipboard.\n");
            free( slashcmd_headcmd_only_temp );
            return 0;
        } // no other args given

        char* temp = ( char* ) malloc( strlen( __slashcmd ) + 1 );
        char* second_arg;
        strcpy( temp , __slashcmd );
        second_arg = strtok( temp , " " );
        second_arg = strtok( NULL , " " );

        if ( strcmp( second_arg , "all" ) == 0 )
        {
            clipboard_copy( last_response );
            crprint( "[dim]Last reply copied to Clipboard.\n");
            free( temp );
        } // /copy all
        else if ( strcmp( second_arg , "code" ) == 0 )
        {
            c_vector codelist;
            cv_init( &codelist , 10 );
            char* selected_code = NULL;
            search_codes( &codelist , last_response );
            if ( cv_len( &codelist ) == 0 )
            {
                crprint( "[dim]No code found.\n" );
                cv_clean( &codelist );
                free( temp );
                free( slashcmd_headcmd_only_temp );
                return 0;
            } // no code found
            if ( cv_len( &codelist ) == 1 )
            {
                selected_code = ( char* ) codelist.items[0];
                goto copy_code;
            } // only one code block exists

            // more than one code block exist

            crprint( "[dim]There are more than one code in ChatGPT's last reply.\n" );
            for ( int i = 0 ; i < cv_len( &codelist ) ; i++ )
            {
                crprint( "[yellow]Code [bright yellow]%d[/]:\n" , i + 1 );
                md_set( ( char* ) codelist.items[i] );
                md_parse();
                md_print( true );
                printf( "\n" );
            }

        ask_code_index:
            disable_history_search();
            char* code_index_str = readline( "Please select which code to copy: " );
            enable_history_search();
            int code_index = atoi( code_index_str );
            if ( code_index > 0 && code_index <= cv_len( &codelist ) )
            {
                selected_code = ( char* ) codelist.items[code_index-1];
                goto copy_code;
            } // legal index input

            printf( "\r\033[2K\r" );
            if ( code_index <= 0 )
                crprint( "[red]Code index must be an integer greater than 0\n" );
            if ( code_index > cv_len( &codelist ) )
                crprint( "[red]Code index out of range: You should input an Integer in range [bright red]1[/] ~ [bright red]%d[/]\n" , cv_len( &codelist ) );
            // illegal index input
            printf( "\033[2A\r\033[2K\r" );
            fflush( stdout );
            free( code_index_str );
            goto ask_code_index;

        copy_code:
            printf( "\r\033[2K\r" );
            char* code_raw = ( char* ) malloc( strlen( selected_code ) );
            char* bpos = strstr( selected_code , "\n" );
            bpos++;
            char* epos = strstr( bpos , "```" );
            // build code begin and end index
            strncpy( code_raw , bpos , epos - bpos );
            code_raw[epos-bpos] = '\0';
            // copy code raw
            clipboard_copy( code_raw );
            crprint( "[dim]Code copied to Clipboard.\n" );

            cv_clean( &codelist );
            free( temp );
            free( code_raw );
        } // /copy code
        else
            crprint( "[dim]Nothing to do. Available copy command: `[bright magenta]/copy code[/]` or `[bright magenta]/copy all[/]`\n" );
        // /copy ?

        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /copy

// ====================================================================================
// ===================================== /version =====================================
// ====================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/version" ) == 0 )
    {
        ezylog_logdebug( logger , "/version command triggered" );
        pthread_mutex_lock( &remote_version_mutex );
        char* local_version_str = ( char* ) malloc( 36 );
        char* remote_version_str = ( char* ) malloc( 36 );
        if ( remote_version == NULL )
            remote_version = "Unknown";
        sprintf( local_version_str , "[blue]Local Version:[/]  %s" , CGPTERM_VERSION );
        sprintf( remote_version_str , "[green]Remote Version:[/] %s" , remote_version );
        crpanel( "Version" , NULL , 28 , NULL , 2 , local_version_str , remote_version_str );
        free( local_version_str );
        free( remote_version_str );
        pthread_mutex_unlock( &remote_version_mutex );
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /version

// =================================================================================
// ===================================== /list =====================================
// =================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/list" ) == 0 )
    {
        ezylog_logdebug( logger , "/list command triggered" );
        char* api_key_hide = ( char* ) malloc( 16 );
        strncpy( api_key_hide , OPENAI_API_KEY , 3 );
        api_key_hide[3] = '\0';
        strcat( api_key_hide , "..." );
        strncat( api_key_hide , OPENAI_API_KEY + strlen( OPENAI_API_KEY ) - 4 , 4 );

        crprint( "[bold]Settings List:\n" );
        crprint( "    [bright magenta]API Host:[/]\t\t\t%s\n"              , openai -> host );
        crprint( "    [bright magenta]OpenAI API Key:[/]\t\t%s\n"          , api_key_hide );
        crprint( "    [bright magenta]Request Timeout:[/]\t\t%.2lfs\n"     , OPENAI_API_TIMEOUT );
        crprint( "    [bright magenta]Use Auto Title Generation:[/]\t%s\n" , AUTO_GENERATE_TITLE ? "Yes" : "No" );
        crprint( "    [bright magenta]Chat History Save Perfix:[/]\t%s\n"  , CHAT_SAVE_PERFIX );
        crprint( "    [bright magenta]Log Level:[/]\t\t\t%s\n"             , parse_priority_to_str( LOG_LEVEL ) );
        crprint( "\n" );
        crprint( "    [bright magenta]Enable Raw Mode:[/]\t\t%s\n"         , raw_mode_enable ? "Yes" : "No" );
        crprint( "    [bright magenta]Enable Stream Mode:[/]\t\t%s\n"      , openai -> stream_mode ? "Yes" : "No" );
        crprint( "    [bright magenta]AI Model:[/]\t\t\t%s\n"              , openai -> model );
        crprint( "    [bright magenta]AI Randomness:[/]\t\t%.2lf\n"        , openai -> temperature );
        free( api_key_hide );
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /list

// ========================================================================================
// ===================================== /help, /exit =====================================
// ========================================================================================

    if ( strcmp( slashcmd_headcmd_only , "/help" ) == 0 )
    {
        ezylog_logdebug( logger , "/help command triggered" );
        print_slash_command_help();
        free( slashcmd_headcmd_only_temp );
        return 0;
    } // /help
    if ( strcmp( slashcmd_headcmd_only , "/exit" ) == 0 )
    {
        ezylog_logdebug( logger , "/exit command triggered" );
        free( slashcmd_headcmd_only_temp );
        return -1;
    } // /exit, ready to break

    // unrecgonized command

    size_t min_levenshtein_distance = strlen( __slashcmd );
    int most_similar_cmd_index = -1;
    for ( int i = 0 ; i < sizeof( slash_commands ) / sizeof( char* ) - 1 ; i++ )
    {
        size_t this_levenshtein_distance = get_levenshtein_distance( __slashcmd , slash_commands[i] );
        // levenshtein distance between __slashcmd and this slash command
        if ( minn( min_levenshtein_distance , this_levenshtein_distance ) < min_levenshtein_distance )
        {
            if ( get_jaccard_similarity( __slashcmd , slash_commands[i] ) >= 0.75 )
            {
                min_levenshtein_distance = this_levenshtein_distance;
                most_similar_cmd_index = i;
            } // update most similar index only if the jaccard similarity of these two greater than 75%
        }
    }
    crprint( "Unrecognized Slash Command `[bright red]%s[/]`." , __slashcmd );
    if ( most_similar_cmd_index != -1 )
        crprint( " Do you mean `[bright magenta]%s[/]`?\n" , slash_commands[most_similar_cmd_index] );
    else
        printf( "\n" );
    crprint( "Use `[bright magenta]/help[/]` to see all available slash commands.\n" );
    free( slashcmd_headcmd_only_temp );
    return 1;
}

void print_slash_command_help(){
    crprint( "[bold]Available commands:\n" );
    crprint( "    [bright magenta]/raw[/]\t\t\t- Toggle raw mode (showing raw text of ChatGPT's reply)\n" );
    crprint( "    [bright magenta]/stream[/]\t\t\t- Toggle stream output mode (flow print the answer)\n" );
    crprint( "    [bright magenta]/title[/]\t\t\t- Toggle whether to enable automatic title generation\n" );
    crprint( "    [bright magenta]/tokens[/]\t\t\t- Show the total tokens spent and the tokens for the current conversation\n" );
    crprint( "    [bright magenta]/usage[/]\t\t\t- Show total credits and current credits used\n" );
    crprint( "    [bright magenta]/timeout[/] [bold]\\[new_timeout][/]\t- Modify the api timeout\n" );
    crprint( "    [bright magenta]/model[/] [bold]\\[model_name][/]\t\t- Change AI model\n" );
    crprint( "    [bright magenta]/system[/] [bold]\\[new_prompt][/]\t- Modify the system prompt\n" );
    crprint( "    [bright magenta]/rand[/] [bold]\\[randomness][/]\t\t- Set Model sampling temperature (0~2)\n" );
    crprint( "    [bright magenta]/save[/] [bold]\\[filename_or_path][/]\t- Save the chat history to a file, suggest title if filename_or_path not provided\n" );
    crprint( "    [bright magenta]/undo[/]\t\t\t- Delete the previous question and answer\n" );
    crprint( "    [bright magenta]/delete[/] [bold](first)[/]\t\t- Delete the first conversation in current chat\n" );
    crprint( "    [bright magenta]/delete[/] [bold]all[/]\t\t\t- Clear current chat, delete all questions and responses\n" );
    crprint( "    [bright magenta]/last[/]\t\t\t- Display last ChatGPT's reply\n" );
    crprint( "    [bright magenta]/copy[/] [bold](all)[/]\t\t\t- Copy the full ChatGPT's last reply (raw) to Clipboard\n" );
    crprint( "    [bright magenta]/copy[/] [bold]code[/]\t\t\t- Copy the code in ChatGPT's last reply to Clipboard\n" );
    crprint( "    [bright magenta]/version[/]\t\t\t- Show cGPTerm local and remote version\n" );
    crprint( "    [bright magenta]/list[/]\t\t\t- List all settings in use\n" );
    crprint( "    [bright magenta]/help[/]\t\t\t- Show this help message\n" );
    crprint( "    [bright magenta]/exit[/]\t\t\t- Exit cGPTerm\n" );
    return;
}