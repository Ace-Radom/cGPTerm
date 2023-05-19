#include"openai_api.h"

openai_t* openai = NULL;
long HTTP_Response_code = 0;

CURL* curl_using_now = NULL;
bool curl_request_abort_called = false;

CURL* title_background_generation_curl = NULL;
pthread_mutex_t openai_title_change_mutex;

typedef struct {
    cdate_t start_date;
    cdate_t end_date;
    double usage;
} get_usage_date_data_transfer_t;

typedef struct {
    char* model;
    int tokens_limit;
} chat_model_list_t;

const chat_model_list_t chat_models[] = {
    { "gpt-4-32k"          , 32768 },
    { "gpt-4-32k-0314"     , 32768 },
    { "gpt-4"              , 8192  },
    { "gpt-4-0314"         , 8192  },
    { "gpt-3.5-turbo"      , 4096  },
    { "gpt-3.5-turbo-0301" , 4096  }
};

void openai_init(){
    openai = ( openai_t* ) malloc( sizeof( openai_t ) );
    openai -> host = OPENAI_HOST;
    openai -> endpoint = ( char* ) malloc( 256 );
    sprintf( openai -> endpoint , "%s/v1/chat/completions" , openai -> host );
    openai -> subscription_endpoint = ( char* ) malloc( 256 );
    sprintf( openai -> subscription_endpoint , "%s/dashboard/billing/subscription" , openai -> host );
    openai -> usage_endpoint = ( char* ) malloc( 256 );
    sprintf( openai -> usage_endpoint , "%s/dashboard/billing/usage" , openai -> host );
    openai -> headers = NULL;
    openai -> messages = json_array();
    openai -> model = ( char* ) malloc( 64 );
    strcpy( openai -> model , "gpt-3.5-turbo" );
    openai -> temperature = 1.0;
    openai -> stream_mode = true;
    openai -> tokens_limit = 4096;
    openai -> total_tokens_spent = 0;
    openai -> current_tokens = 0;
    openai -> title = NULL;
    openai -> credit_total_granted = 0.0;
    openai -> credit_total_used = 0.0;
    openai -> credit_used_this_month = 0.0;
    openai -> credit_plan = NULL;
    // basic init

    ezylog_logdebug( logger , "OpenAI Chat Endpoint: %s" , openai -> endpoint );
    ezylog_logdebug( logger , "OpenAI Subcription Endpoint: %s" , openai -> subscription_endpoint );
    ezylog_logdebug( logger , "OpenAI Usage Endpoint: %s" , openai -> usage_endpoint );

    char* authorization_bearer_header_str = ( char* ) malloc( 128 );
    sprintf( authorization_bearer_header_str , "%s%s" , "Authorization: Bearer " , OPENAI_API_KEY );
    openai -> headers = curl_slist_append( openai -> headers , "Content-Type: application/json" );
    openai -> headers = curl_slist_append( openai -> headers , authorization_bearer_header_str );
    // headers init

    json_t* prompt = json_object();
    json_object_set_new( prompt , "role" , json_string( "system" ) );
    json_object_set_new( prompt , "content" , json_string( "You are a helpful assistant. Knowledge cutoff: 2021-09" ) );
    json_array_append_new( openai -> messages , prompt );
    // prompt init

    count_tokens_init();
    // tokens counter init

    openai -> current_tokens += count_tokens_message( prompt );
    // count prompt tokens and add to current_tokens

    pthread_mutex_init( &openai_title_change_mutex , NULL );
    // mutex init

    return;
}

void* openai_send_chatrequest( void* __data ){
    request_working = true;
    openai_datatransfer_t* data = ( openai_datatransfer_t* ) __data;
    const char* __usrmsg = data -> msg;
    // unpack transfer data

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { openai -> stream_mode ? OPENAI_STREAM_REQUEST : OPENAI_NORMAL_REQUEST , NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "[openai_send_chatrequest] -> curl init failed\n" );
        ezylog_logerror( logger , "curl init failed when sending chat request" );
        request_working = false;
        return NULL;
    } // curl init error

    ezylog_loginfo( logger , "> %s" , __usrmsg );
    const char* text;
    // return var

    json_t* new_usrmsg = json_object();
    json_object_set_new( new_usrmsg , "role" , json_string( "user" ) );
    json_object_set_new( new_usrmsg , "content" , json_string( __usrmsg ) );
    json_array_append_new( openai -> messages , new_usrmsg );
    // append new user msg to messages
    json_t* request_json_root = json_object();
    json_object_set_new( request_json_root , "model" , json_string( openai -> model ) );
    json_object_set_new( request_json_root , "messages" , json_copy( openai -> messages ) );
    // copy a new message list, in order to free request_json_root and not destory original list
    json_object_set_new( request_json_root , "temperature" , json_real( openai -> temperature ) );
    json_object_set_new( request_json_root , "stream" , json_boolean( openai -> stream_mode ) );
    char* request_data = json_dumps( request_json_root , JSON_COMPACT );
    ezylog_logdebug( logger , "Request raw: %s" , request_data );
    // make request data

    curl_easy_setopt( curl , CURLOPT_URL , openai -> endpoint );
    curl_easy_setopt( curl , CURLOPT_HTTPHEADER , openai -> headers );
    curl_easy_setopt( curl , CURLOPT_POSTFIELDS , request_data );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    curl_using_now = curl;
    // raise the curl function in progress now

    if ( openai -> stream_mode )
    {
        stream_response_msg_only_buf = ( char* ) malloc( 1024 );
        stream_response_msg_only_buf_size = 1024;
        strcpy( stream_response_msg_only_buf , "" );
    }

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        usleep( 10000 );
        // sleep 10ms in order to wait curl_request_abort_called set to true by openai_request_abort (if abort called)
        if ( curl_request_abort_called )
            goto request_stop;
        // res not CURLE_OK because of called abort
        crprint( "[red]send request failed: %s\n" , curl_easy_strerror( res ) );
        ezylog_logerror( logger , "send request failed: %s" , curl_easy_strerror( res ) );
        text = NULL;
        goto request_stop;
    }

    ezylog_logdebug( logger , "Response got, total size: %ld" , response_data.size );

    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &HTTP_Response_code );
    // get api response code and parse it later

    json_t* root;
    json_error_t error;

    if ( !openai -> stream_mode )
    {
        root = json_loads( response_data.ptr , 0 , &error );
        if ( !root )
        {
            fprintf( stderr , "[openai_send_chatrequest] -> response json error at line %d: %s\n" , error.line , error.text );
            ezylog_logerror( logger , "response json error at line %d: %s" , error.line , error.text );
            ezylog_logerror( logger , "Response raw: %s" , response_data.ptr );
            text = NULL;
            goto request_stop;
        } // json error
    
        if ( HTTP_Response_code / 100 == 4 )
        {
            json_t* response_errormsg = json_object_get( root , "error" );
            text = json_string_value( json_object_get( response_errormsg , "message" ) );
            ezylog_logerror( logger , "OpenAI API responsed Error: Code %ld, Message: \"%s\"" , HTTP_Response_code , text );
            ezylog_logdebug( logger , "GPT Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
            goto request_stop;
            // because of unknown format problem (openai's error response contains \n), using json_dumps here
        } // parse response code, match 4xx errros
        else
        {
            json_t* response_msg = json_object_get( root , "choices" );
            response_msg = json_array_get( response_msg , 0 );
            response_msg = json_object_get( response_msg , "message" );
            json_array_append_new( openai -> messages , response_msg );
            text = json_string_value( json_object_get( response_msg , "content" ) );
            ezylog_loginfo( logger , "ChatGPT: %s" , text );
            ezylog_logdebug( logger , "GPT Response raw: %s" , response_data.ptr );
            openai -> current_tokens = json_integer_value( json_object_get( json_object_get( root , "usage" ) , "total_tokens" ) );
            openai -> total_tokens_spent += openai -> current_tokens;
            ezylog_loginfo( logger , "Tokens used this turn: %ld" , openai -> current_tokens );
            // count tokens
        } // response code 200 OK (most likely)
    } // normal request mode (not stream)
    else
    {
        if ( HTTP_Response_code / 100 == 4 )
        {
            root = json_loads( response_data.ptr , 0 , &error );
            if ( !root )
            {
                fprintf( stderr , "[openai_send_chatrequest] -> response json error at line %d: %s\n" , error.line , error.text );
                ezylog_logerror( logger , "response json error at line %d: %s" , error.line , error.text );
                ezylog_logerror( logger , "Response raw: %s" , response_data.ptr );
                text = NULL;
                goto request_stop;
            }
            json_t* response_errormsg = json_object_get( root , "error" );
            text = json_string_value( json_object_get( response_errormsg , "message" ) );
            ezylog_logerror( logger , "OpenAI API responsed Error: Code %ld, Message: \"%s\"" , HTTP_Response_code , text );
            ezylog_logdebug( logger , "GPT Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
            free( stream_response_msg_only_buf );
            goto request_stop;
        } // parse response code, match 4xx errors
        // OpenAI also responses json when error occurs
        else
        {
            if ( !raw_mode_enable )
            {
                printf( "\033[%ld;%ldH\033[2K\033[J\r" , print_begin_cursor_y , print_begin_cursor_x );
                fflush( stdout );
                // clean output before
                md_set( stream_response_msg_only_buf );
                md_parse();
                md_print( true );
                // print the last block
            }

            ezylog_loginfo( logger , "ChatGPT: %s" , stream_response_msg_only_buf );
            json_t* new_responsemsg = json_object();
            json_object_set_new( new_responsemsg , "role" , json_string( "assistant" ) );
            json_object_set_new( new_responsemsg , "content" , json_string( stream_response_msg_only_buf ) );
            json_array_append_new( openai -> messages , new_responsemsg );
            // append new response to message list
            printf( "\n" );
            openai_count_tokens();
            openai -> total_tokens_spent += openai -> current_tokens;
            ezylog_loginfo( logger , "Tokens used this turn: %ld" , openai -> current_tokens );
            // count current tokens
            free( stream_response_msg_only_buf );
        } // no error, stream has already been printed
    } // stream mode

    // here: check if need to generate title background
    // but if error occurs, there's no need to generate a title
    // therefore add goto request_stop after each error part

    if ( AUTO_GENERATE_TITLE && json_array_size( openai -> messages ) == 3 )
    {
        const char* first_msg = json_string_value( json_object_get( json_array_get( openai -> messages , 1 ) , "content" ) );

        pthread_t generate_title_background;
        pthread_attr_t generate_title_background_attr;
        pthread_attr_init( &generate_title_background_attr );
        pthread_attr_setdetachstate( &generate_title_background_attr , PTHREAD_CREATE_DETACHED );
        ezylog_logdebug( logger , "title background generation triggered, call generate function" );
        pthread_create( &generate_title_background , &generate_title_background_attr , openai_generate_title , ( void* ) first_msg );
        pthread_attr_destroy( &generate_title_background_attr );
    } // use auto generate title, and messages length is 3 (system prompt, first request, first response)

request_stop:
    if ( !curl_request_abort_called )
        curl_easy_cleanup( curl );
    // when abort called, curl has already been cleaned up
    free( request_data );
    free( response_data.ptr );
    json_decref( request_json_root );
    data -> response = text;
    curl_using_now = NULL;
    request_working = false;
    return NULL;
}

void openai_free(){
    curl_slist_free_all( openai -> headers );
    json_decref( openai -> messages );
    free( openai -> endpoint );
    free( openai -> subscription_endpoint );
    free( openai -> usage_endpoint );
    free( openai -> model );
    if ( openai -> title )
        free( openai -> title );
    free( openai );
    pthread_mutex_destroy( &openai_title_change_mutex );
    return;
}

void openai_count_tokens(){
    char* whole_msg = ( char* ) malloc( 4096 );
    size_t size = 4096;
    strcpy( whole_msg , "" );
    for ( int i = 0 ; i < json_array_size( openai -> messages ) ; i++ )
    {
        json_t* this_msg = json_array_get( openai -> messages , i );
        const char* role = json_string_value( json_object_get( this_msg , "role" ) );
        const char* content = json_string_value( json_object_get( this_msg , "content" ) );
        char* this_msg_str = ( char* ) malloc( strlen( role ) + strlen( content ) + 32 );
        sprintf( this_msg_str , "role: %s, content: %s" , role , content );
        // build this message's str
        if ( strlen( this_msg_str ) + strlen( whole_msg ) > size )
        {
            whole_msg = realloc( whole_msg , size + strlen( this_msg_str ) + 1 );
            size += strlen( this_msg_str );
        } // whole_msg cannot contain this msg
        strcat( whole_msg , this_msg_str );
        free( this_msg_str );
    } // build whole message str
    openai -> current_tokens = count_tokens( whole_msg );
    free( whole_msg );
    return;
}

int openai_set_model( char* __new_model ){
    char* str = ( char* ) malloc( strlen( __new_model ) + 1 );
    strcpy( str , __new_model );
    char* new_model_alllower = str;
    for ( ; *str != '\0' ; str++ )
        *str = tolower( *str );
    // get all lower new model name
    int index = -1;
    for ( int i = 0 ; i < sizeof( chat_models ) / sizeof( chat_model_list_t ) ; i++ )
        if ( strcmp( new_model_alllower , chat_models[i].model ) == 0 )
        {
            index = i;
            break;
        } // found
    free( new_model_alllower );
    if ( index == -1 )
        return -1;
    // not found, __new_model is illegal
    strcpy( openai -> model , chat_models[index].model );
    openai -> tokens_limit = chat_models[index].tokens_limit;
    return 0;
}

void openai_set_prompt( char* __new_prompt ){
    if ( __new_prompt == NULL )
        return;

    json_t* prompt = json_array_get( openai -> messages , 0 );
    json_object_set_new( prompt , "content" , json_string( __new_prompt ) );
    return;
}

void openai_set_temperature( double __new_temperature ){
    openai -> temperature = __new_temperature;
    return;
}

int openai_save_history( FILE* __f ){
    return json_dumpf( openai -> messages , __f , JSON_INDENT( 4 ) );;
}

void openai_load_history( const char* __history_file ){
    ezylog_logdebug( logger , "load history triggered" );
    FILE* savef = fopen( __history_file , "r" );
    if ( savef == NULL )
    {
        crprint( "[red]Failed to load chat history from '[bold]%s[/]', check log for more informations\n" , __history_file );
        char* errmsg = strerror( errno );
        // get error message
        ezylog_logerror( logger , "Failed to load chat history from '%s': errno %d, error message \"%s\"" , __history_file , errno , errmsg );
        return;
    } // open file failed

    json_t* history;
    json_error_t error;
    
    history = json_loadf( savef , JSON_DISABLE_EOF_CHECK , &error );
    if ( !history )
    {
        crprint( "[red]Failed to load chat history from '[bold]%s[/]' due to illegal format, check log for more information\n" , __history_file );
        ezylog_logerror( logger , "Failed to load chat history from '%s' due to illegal format: at line %d: %s" , __history_file , error.line , error.text );
        fclose( savef );
        return;
    } // illegal json format

    openai -> messages = json_copy( history );

    json_t* this_msg;
    for ( int i = 1 ; i < json_array_size( openai -> messages ) ; i++ ) // system prompt should never be print
    {
        this_msg = json_array_get( openai -> messages , i );
        const char* role = json_string_value( json_object_get( this_msg , "role" ) );
        const char* content = json_string_value( json_object_get( this_msg , "content" ) );
        if ( strcmp( role , "user" ) == 0 )
        {
            printf( "> %s\n" , content );
        } // this msg is a user input
        else
        {
            crprint( "[bold][bright cyan]ChatGPT:\n" );
            if ( raw_mode_enable )
            {
                printf( "%s\n" , content );
            }
            else
            {
                md_set( content );
                md_parse();
                md_print( true );
                printf( "\n" );
            }
            
        } // this msg is a ChatGPT response
    } // print history message
    crprint( "[dim]Chat history successfully loaded from: [bright magenta]%s[/]\n" , __history_file );
    ezylog_loginfo( logger , "Chat history successfully loaded from: \"%s\"" , __history_file );
    printf( "\033]0;%s\007" , __history_file );
    fflush( stdout );
    // change CLI title
    json_decref( history );
    fclose( savef );
    return;
}

void* openai_get_subscription(){
    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { OPENAI_USAGE_GET_REQUEST , NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when getting subscription" );
        openai -> credit_total_granted = -1;
        openai -> credit_plan = "Unknown";
        request_working = false;
        return NULL;
    } // curl init error

    curl_easy_setopt( curl , CURLOPT_HTTPGET , 1L );
    curl_easy_setopt( curl , CURLOPT_URL , openai -> subscription_endpoint );
    curl_easy_setopt( curl , CURLOPT_HTTPHEADER , openai -> headers );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        ezylog_logerror( logger , "send request failed when getting subscription: %s" , curl_easy_strerror( res ) );
        openai -> credit_total_granted = -1;
        openai -> credit_plan = "Unknown";
        goto request_stop;
    }

    long response_code;
    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        ezylog_logerror( logger , "when getting subscription, response json error at line %d: %s" , error.line , error.text );
        ezylog_logerror( logger , "Response raw: %s" , response_data.ptr );
        openai -> credit_total_granted = -1;
        openai -> credit_plan = "Unknown";
        goto request_stop;
    } // json error

    if ( response_code / 100 == 4 )
    {
        json_t* response_errormsg = json_object_get( root , "error" );
        ezylog_logerror( logger , "OpenAI subscription API responsed Error: Code %ld, Message: \"%s\"" , response_code , json_string_value( json_object_get( response_errormsg , "message" ) ) );
        ezylog_logdebug( logger , "Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
        openai -> credit_total_granted = -1;
        openai -> credit_plan = "Unknown";
    } // parse response code, match 4xx errors
    else
    {
        openai -> credit_total_granted = json_real_value( json_object_get( root , "hard_limit_usd" ) );
        json_t* title_obj = json_copy( json_object_get( json_object_get( root , "plan" ) , "title" ) );
        openai -> credit_plan = json_string_value( title_obj );
        ezylog_logdebug( logger , "OpenAI subcription got: credit total granted: %lf, credit plan: \"%s\"" , openai -> credit_total_granted , openai -> credit_plan );
    } // response code 200 OK (most likely)
    
    json_decref( root );

request_stop:
    curl_easy_cleanup( curl );
    free( response_data.ptr );
    return NULL;
}

void* openai_get_usage( void* __be_data ){
    get_usage_date_data_transfer_t* be_data = ( get_usage_date_data_transfer_t* ) __be_data;
    char* start_date = parse_date( be_data -> start_date );
    char* end_date = parse_date( be_data -> end_date );
    // unpack transfer data

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { OPENAI_USAGE_GET_REQUEST , NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when getting usage between %s and %s" , start_date , end_date );
        be_data -> usage = -1;
        return NULL;
    } // curl init error

    char* get_usage_url = ( char* ) malloc( 256 );
    sprintf( get_usage_url , "%s?start_date=%s&end_date=%s" , openai -> usage_endpoint , start_date , end_date );
    // build get url

    curl_easy_setopt( curl , CURLOPT_HTTPGET , 1L );
    curl_easy_setopt( curl , CURLOPT_URL , get_usage_url );
    curl_easy_setopt( curl , CURLOPT_HTTPHEADER , openai -> headers );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    double usage_return = 0;

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        ezylog_logerror( logger , "send request failed when getting usage between %s and %s" , start_date , end_date );
        usage_return = -1;
        goto request_stop;
    }

    long response_code;
    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        ezylog_logerror( logger , "when getting usage between %s and %s, response json error at line %d: %s" , start_date , end_date , error.line , error.text );
        ezylog_logerror( logger , "Response raw: %s" , response_data.ptr );
        usage_return = -1;
        goto request_stop;
    } // json error

    if ( response_code / 100 == 4 )
    {
        json_t* response_errormsg = json_object_get( root , "error" );
        ezylog_logerror( logger , "OpenAI usage API responsed Error (%s ~ %s): Code %ld, Message: \"%s\"" , start_date , end_date , response_code , json_string_value( json_object_get( response_errormsg , "message" ) ) );
        ezylog_logdebug( logger , "Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
        usage_return = -1;
    } // parse response code, match 4xx errors
    else
    {
        usage_return = json_real_value( json_object_get( root , "total_usage" ) );
        usage_return /= 100.0;
        ezylog_logdebug( logger , "OpenAI usage got: %lf$ between %s and %s" , usage_return , start_date , end_date );
    } // response code 200 OK (most likely)

    json_decref( root );

request_stop:
    curl_easy_cleanup( curl );
    free( response_data.ptr );
    free( get_usage_url );
    free( start_date );
    free( end_date );
    be_data -> usage = usage_return;
    return NULL;
}

void* openai_get_usage_summary(){
    request_working = true;

    openai -> credit_total_granted = 0;
    openai -> credit_used_this_month = 0;
    openai -> credit_total_used = 0;
    openai -> credit_plan = NULL;

    get_usage_date_data_transfer_t usage_this_month;
    get_today_date( &usage_this_month.end_date );
    usage_this_month.start_date = usage_this_month.end_date;
    usage_this_month.start_date.day = 1;
    date_add_day( &usage_this_month.end_date , 1 );
    usage_this_month.usage = 0;
    // build the time span of this month

    pthread_t get_subscription;
    pthread_t get_usage_this_month;
    pthread_create( &get_subscription , NULL , openai_get_subscription , NULL );
    pthread_create( &get_usage_this_month , NULL , openai_get_usage , ( void* ) &usage_this_month );
    // start to get credit plan, credit total granted and credit usage this month

// ================================ start to get total usage ================================

    void* get_total_usage_thread_pool = pool_start( openai_get_usage , 5 );
    // create thread pool
    c_vector get_total_usage_date_span;
    cv_init( &get_total_usage_date_span , 10 );
    // create args vector

    cdate_t today;
    cdate_t start_date = { 2023 , 1 , 1 };
    cdate_t end_date = start_date;
    get_today_date( &today );
    while ( date_diff( start_date , today ) > 0 )
    {
        date_add_day( &end_date , 99 );
        // date span max is 99 days

        get_usage_date_data_transfer_t* usage_this_span = ( get_usage_date_data_transfer_t* ) malloc( sizeof( get_usage_date_data_transfer_t ) );
        usage_this_span -> start_date = start_date;
        usage_this_span -> end_date = end_date;
        usage_this_span -> usage = 0;
        cv_push_back( &get_total_usage_date_span , ( void* ) usage_this_span );
        // build new time span data, and push it to vector
        start_date = end_date;
    }

    for ( int i = 0 ; i < cv_len( &get_total_usage_date_span ) ; i++ )
        pool_enqueue( get_total_usage_thread_pool , get_total_usage_date_span.items[i] , 0 );
    // start all jobs
    pool_wait( get_total_usage_thread_pool );
    // wait all jobs to be finished

    for ( int i = 0 ; i < cv_len( &get_total_usage_date_span ) ; i++ )
    {
        get_usage_date_data_transfer_t* usage_this_span = ( get_usage_date_data_transfer_t* ) get_total_usage_date_span.items[i];
        openai -> credit_total_used += usage_this_span -> usage;
    } // unpack datas and add all stage usage to credit_total_used
    pool_end( get_total_usage_thread_pool );
    cv_clean( &get_total_usage_date_span );
    // clean up

    pthread_join( get_subscription , NULL );
    pthread_join( get_usage_this_month , NULL );
    openai -> credit_used_this_month = usage_this_month.usage;
    request_working = false;
    return NULL;
}

void* openai_generate_title( void* __data ){
    const char* __msg_used_to_gen_title = ( char* ) __data;
    char* msg_used_to_gen_title = ( char* ) malloc( strlen( __msg_used_to_gen_title ) + 1 );
    strcpy( msg_used_to_gen_title , __msg_used_to_gen_title );
    // get message used to generate title

    // copy msg transfered-in in order to prevent original msg be free before it's used here

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { OPENAI_BACKGROUND_TITLE_GENERATION , NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when generating title background" );
        free( msg_used_to_gen_title );
        return NULL;
    } // curl init error

    ezylog_loginfo( logger , "start to generate title background..." );

    char* prompt = ( char* ) malloc( strlen( msg_used_to_gen_title ) + strlen( GEN_TITLE_PROMPT ) + 32 );
    strcpy( prompt , GEN_TITLE_PROMPT );
    strcat( prompt , "\n\nContent:\"\n" );
    strcat( prompt , msg_used_to_gen_title );
    strcat( prompt , "\n\"" );

    json_t* request_msg = json_object();
    json_object_set_new( request_msg , "role" , json_string( "user" ) );
    json_object_set_new( request_msg , "content" , json_string( prompt ) );
    json_t* request_msg_array = json_array();
    json_array_append_new( request_msg_array , request_msg );
    // build request message

    json_t* request_json_root = json_object();
    json_object_set_new( request_json_root , "model" , json_string( "gpt-3.5-turbo" ) );
    json_object_set_new( request_json_root , "messages" , request_msg_array );
    json_object_set_new( request_json_root , "temperature" , json_real( 0.5 ) );
    char* request_data = json_dumps( request_json_root , JSON_COMPACT );
    ezylog_logdebug( logger , "Generate Title Request raw: %s" , request_data );
    // make request data

    curl_easy_setopt( curl , CURLOPT_URL , openai -> endpoint );
    curl_easy_setopt( curl , CURLOPT_HTTPHEADER , openai -> headers );
    curl_easy_setopt( curl , CURLOPT_POSTFIELDS , request_data );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , &response_data );
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , curl_write_callback_function );
    curl_easy_setopt( curl , CURLOPT_TIMEOUT_MS , ( int ) ( OPENAI_API_TIMEOUT * 1000 ) );
    // make curl request

    title_background_generation_curl = curl;
    // raise title background generation's curl

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        ezylog_logerror( logger , "send request failed when generating title background: %s" , curl_easy_strerror( res ) );
        goto request_stop;
    }

    ezylog_logdebug( logger , "Generate Title Response got, total size: %ld" , response_data.size );

    long response_code;
    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &response_code );
    // get api response code and parse it later

    json_t* root;
    json_error_t error;

    root = json_loads( response_data.ptr , 0 , &error );
    if ( !root )
    {
        ezylog_logerror( logger , "generate title response json error at line %d: %s" , error.line , error.text );
        ezylog_logerror( logger , "Generate Title Response raw: %s" , response_data.ptr );
        goto request_stop;
    } // json error

    if ( response_code / 100 == 4 )
    {
        json_t* response_errormsg = json_object_get( root , "error" );
        const char* errormsg = json_string_value( json_object_get( response_errormsg , "message" ) );
        ezylog_logerror( logger , "OpenAI API responsed Error when generating title background: Code %ld, Message: \"%s\"" , response_code , errormsg );
        ezylog_logdebug( logger , "GPT Response raw: %s" , json_dumps( root , JSON_COMPACT ) );
    } // parse response code, match 4xx errors
    else
    {
        json_t* response_msg = json_object_get( root , "choices" );
        response_msg = json_array_get( response_msg , 0 );
        response_msg = json_object_get( response_msg , "message" );
        const char* title_generated = json_string_value( json_object_get( response_msg , "content" ) );
        ezylog_loginfo( logger , "Title Generated: %s" , title_generated );
        ezylog_logdebug( logger , "GPT Response raw: %s" , response_data.ptr );
        long tokens_spent = json_integer_value( json_object_get( json_object_get( root , "usage" ) , "total_tokens" ) );
        openai -> total_tokens_spent += tokens_spent;
        // add title generation tokens cost to total tokens spent
        ezylog_loginfo( logger , "Tokens used to generate title: %ld" , tokens_spent );

        pthread_mutex_lock( &openai_title_change_mutex );

        if ( openai -> title != NULL )
        {
            free( openai -> title );
            openai -> title = NULL;
        } // already generated title once
        char* title_temp = ( char* ) malloc( strlen( title_generated ) + 1 );
        strcpy( title_temp , title_generated );
        openai -> title = title_temp;
        // title_generated (which is taken from json objects) will be free when destroying json object, therefore copy it

        printf( "\033]0;%s\007" , openai -> title );
        fflush( stdout );
        // change CLI title

        pthread_mutex_unlock( &openai_title_change_mutex );

    } // response code 200 OK (most likely)

request_stop:
    curl_easy_cleanup( curl );
    free( msg_used_to_gen_title );
    free( prompt );
    free( request_data );
    free( response_data.ptr );
    json_decref( request_json_root );
    json_decref( root );
    title_background_generation_curl = NULL;
    return NULL;
}

void openai_request_abort(){
    if ( curl_using_now == NULL )
        return;
    // no curl is working
    curl_easy_cleanup( curl_using_now );
    curl_using_now = NULL;
    curl_request_abort_called = true;
    // raise request abort
    // signal reset function is in CLI main loop
    ezylog_loginfo( logger , "request abort called" );
    return;
}

void openai_undo(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        crprint( "[dim]Nothing to undo\n" );
        return;
    }
    const char* last_msg = json_string_value( json_object_get( json_array_get( openai -> messages , msglist_size - 2 ) , "content" ) );
    crprint( "[dim]Last question: '[green]%s[/]' and its answer has been removed.\n" , last_msg );
    openai_msg_popback();
    openai_msg_popback();
    return;
}

void openai_delete_first(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        crprint( "[dim]Nothing to delete\n" );
        return;
    }
    const char* first_msg = json_string_value( json_object_get( json_array_get( openai -> messages , 1 ) , "content" ) );
    crprint( "[dim]First question: '[green]%s[/]' and its answer has been deleted\n" , first_msg );
    json_array_remove( openai -> messages , 1 );
    json_array_remove( openai -> messages , 1 );
    // delete first request and response
    return;
}

void openai_delete_all(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        crprint( "[dim]Nothing to delete\n" );
        return;
    }
    while ( json_array_size( openai -> messages ) > 1 )
        json_array_remove( openai -> messages , 1 );
    // remove all except prompt
    crprint( "[dim]Current chat deleted.\n" );
    return;
}

size_t openai_get_message_list_length(){
    return json_array_size( openai -> messages );
}

/**
 * @brief get message list's first request content
*/
const char* openai_getfirst(){
    if ( json_array_size( openai -> messages ) <= 1 )
    {
        return NULL;
    }
    const char* first_request = json_string_value( json_object_get( json_array_get( openai -> messages , 1 ) , "content" ) );
    return first_request;
}

/**
 * @brief get message list's last response content
*/
const char* openai_getlast(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        return NULL;
    }
    const char* last_response = json_string_value( json_object_get( json_array_get( openai -> messages , msglist_size - 1 ) , "content" ) );
    return last_response;
}

void openai_msg_popback(){
    size_t msglist_size = json_array_size( openai -> messages );
    if ( msglist_size <= 1 )
    {
        return;
    } // here: normally msglist size can not be 0 because of the prompt message;
      // anyway when there's only prompt message in the list then it can not be pop.
    json_array_remove( openai -> messages , msglist_size - 1 );
    return;
}