#include"openai_api.h"

openai_t* openai = NULL;
bool request_working = false;
long HTTP_Response_code = 0;

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
    openai -> endpoint = "https://api.openai.com/v1/chat/completions";
    openai -> headers = NULL;
    openai -> messages = json_array();
    openai -> model = ( char* ) malloc( 64 );
    strcpy( openai -> model , "gpt-3.5-turbo" );
    openai -> tokens_limit = 4096;
    openai -> total_tokens_spent = 0;
    openai -> current_tokens = 0;
    openai -> title = NULL;
    openai -> credit_total_granted = 0.0;
    openai -> credit_total_used = 0.0;
    openai -> credit_used_this_month = 0.0;
    openai -> credit_plan = NULL;
    // basic init

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

    openai -> current_tokens += count_tokens_message( prompt );
    // count prompt tokens and add to current_tokens

    return;
}

void openai_send_chatrequest( void* __data ){
    request_working = true;
    openai_datatransfer_t* data = ( openai_datatransfer_t* ) __data;
    const char* __usrmsg = data -> msg;
    // unpack transfer data

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        fprintf( stderr , "[openai_send_chatrequest] -> curl init failed\n" );
        ezylog_logerror( logger , "curl init failed when sending chat request" );
        request_working = false;
        return;
    } // curl init error

    ezylog_loginfo( logger , "> %s" , __usrmsg );
    char* text;
    // return var

    json_t* new_usrmsg = json_object();
    json_object_set_new( new_usrmsg , "role" , json_string( "user" ) );
    json_object_set_new( new_usrmsg , "content" , json_string( __usrmsg ) );
    json_array_append_new( openai -> messages , new_usrmsg );
    // append new user msg to messages
    json_t* request_json_root = json_object();
    json_object_set_new( request_json_root , "model" , json_string( openai -> model ) );
    json_object_set_new( request_json_root , "messages" , openai -> messages );
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

    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        fprintf( stderr , "send request failed: %s\n" , curl_easy_strerror( res ) );
        ezylog_logerror( logger , "send request failed: %s" , curl_easy_strerror( res ) );
        text = NULL;
        goto request_stop;
    }

    ezylog_logdebug( logger , "Response got, total size: %ld" , response_data.size );

    curl_easy_getinfo( curl , CURLINFO_RESPONSE_CODE , &HTTP_Response_code );
    // get api response code and parse it later

    json_t* root;
    json_error_t error;

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
        // count tokens
    } // response code 200 OK (most likely)

request_stop:
    curl_easy_cleanup( curl );
    free( request_data );
    free( response_data.ptr );
    data -> response = text;
    request_working = false;
    return;
}

void openai_free(){
    curl_slist_free_all( openai -> headers );
    json_decref( openai -> messages );
    free( openai -> model );
    if ( openai -> title )
        free( openai -> title );
    free( openai );
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
        char* role = json_string_value( json_object_get( this_msg , "role" ) );
        char* content = json_string_value( json_object_get( this_msg , "content" ) );
        if ( strcmp( role , "user" ) == 0 )
        {
            printf( "> %s\n" , content );
        } // this msg is a user input
        else
        {
            crprint( "[bold][bright cyan]ChatGPT:\n" );
            printf( "%s\n" , content );
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

void openai_get_subscription(){
    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when getting subscription" );
        openai -> credit_total_granted = -1;
        openai -> credit_plan = "Unknown";
        request_working = false;
        return;
    } // curl init error

    curl_easy_setopt( curl , CURLOPT_HTTPGET , 1L );
    curl_easy_setopt( curl , CURLOPT_URL , "https://api.openai.com/dashboard/billing/subscription" );
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
    return;
}

void openai_get_usage( void* __be_data ){
    get_usage_date_data_transfer_t* be_data = ( get_usage_date_data_transfer_t* ) __be_data;
    char* start_date = parse_date( be_data -> start_date );
    char* end_date = parse_date( be_data -> end_date );
    // unpack transfer data

    CURL* curl;
    CURLcode res;
    curl_data_t response_data = { NULL , 0 };
    curl = curl_easy_init();
    if ( !curl )
    {
        ezylog_logerror( logger , "curl init failed when getting usage between %s and %s" , start_date , end_date );
        be_data -> usage = -1;
        return;
    } // curl init error

    char* get_usage_url = ( char* ) malloc( 128 );
    strcpy( get_usage_url , "https://api.openai.com/dashboard/billing/usage" );
    strcat( get_usage_url , "?start_date=" );
    strcat( get_usage_url , start_date );
    strcat( get_usage_url , "&end_date=" );
    strcat( get_usage_url , end_date );
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
    return;
}

void openai_get_usage_summary(){
    request_working = true;

    openai -> credit_total_granted = 0;
    openai -> credit_used_this_month = 0;
    openai -> credit_total_used = 0;
    openai -> credit_plan = NULL;

    get_usage_date_data_transfer_t usage_this_month;
    get_today_date( &usage_this_month.end_date );
    usage_this_month.start_date = usage_this_month.end_date;
    usage_this_month.start_date.day = 1;
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
    return;
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