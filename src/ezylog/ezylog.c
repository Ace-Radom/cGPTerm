#include"ezylog.h"

ezylog_logger_t* ezylog_init( const char* __name , const char* __layout , const char* __logfile , const ezylog_logmode_t __mode , const ezylog_priority_t __priority ){
    ezylog_logger_t* new_logger = ( ezylog_logger_t* ) malloc( sizeof( ezylog_logger_t ) );
    strcpy( new_logger -> __name , __name );
    strcpy( new_logger -> __layout , __layout );
    if ( __mode == EZYLOG_LOGMODE_APPEND )
        new_logger -> __f = fopen( __logfile , "a" );
        // append mode
    else
        new_logger -> __f = fopen( __logfile , "w" );
        // new mode

    if ( new_logger -> __f == NULL )
    {
        fprintf( stderr , "[ezylog_init] -> Open log file \"%s\" failed\n" , __logfile );
        free( new_logger );
        return NULL;
    }
    if ( pthread_mutex_init( &new_logger -> __mutex , NULL ) != 0 )
    {
        fprintf( stderr , "[ezylog_init] -> init mutex failed\n" );
        free( new_logger );
        return NULL;
    }

    new_logger -> __pt = __priority;
    return new_logger;
}

void ezylog_logdebug( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return;
    }

    if (  __logger -> __pt < EZYLOG_PRIORITY_DEBUG )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_DEBUG , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return;
}

void ezylog_loginfo( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_INFO )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_INFO , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return;
}

void ezylog_logerror( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_ERROR )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_ERROR , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return;
}

void ezylog_logfatal( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_FATAL )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_FATAL , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return;
}

void ezylog_close( ezylog_logger_t* __logger ){
    fclose( __logger -> __f );
    free( __logger );
    return;
}