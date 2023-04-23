#include"ezylog.h"

/**
 * @brief ezylog logger initialize
 * 
 * @param __name the name of this logger
 * @param __layout the layout of all logs written by this logger
 * @param __logfile linked log file's name
 * @param __mode log file open mode (create a new one / overlay an old one; append to an old one)
 * @param __pt log priority (DEBUG, INFO, ERROR, FATAL)
 * 
 * @throw when open log file failed, print error to stderr and return NULL
 * @throw when initialize mutex failed, print error to stderr return NULL
 * 
 * @return an initialized ezylog logger (ezylog_logger_t*)
*/
ezylog_logger_t* ezylog_init( const char* __name , const char* __layout , const char* __logfile , const ezylog_logmode_t __mode , const ezylog_priority_t __pt ){
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

    new_logger -> __pt = __pt;
    return new_logger;
}

/**
 * @brief log debug message to a logger
 * 
 * @param __logger ezylog logger
 * @param __msg debug log message
*/
int ezylog_logdebug( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return EL_MUTEX_LOCK_FAILED;
    }

    if (  __logger -> __pt < EZYLOG_PRIORITY_DEBUG )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return EL_PT_BELOW_SET;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_DEBUG , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return 0;
}

/**
 * @brief log info message to a logger
 * 
 * @param __logger ezylog logger
 * @param __msg info log message
*/
int ezylog_loginfo( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return EL_MUTEX_LOCK_FAILED;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_INFO )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return EL_PT_BELOW_SET;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_INFO , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return 0;
}

/**
 * @brief log error message to a logger
 * 
 * @param __logger ezylog logger
 * @param __msg error log message
*/
int ezylog_logerror( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return EL_MUTEX_LOCK_FAILED;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_ERROR )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return EL_PT_BELOW_SET;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_ERROR , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return 0;
}

/**
 * @brief log fatal message to a logger
 * 
 * @param __logger ezylog logger
 * @param __msg fatal log message
*/
int ezylog_logfatal( ezylog_logger_t* __logger , const char* __msg ){
    if ( pthread_mutex_lock( &( __logger -> __mutex ) ) != 0 )
    {
        fprintf( stderr , "[ezylog_loginfo] -> lock mutex failed, log stop\n" );
        return EL_MUTEX_LOCK_FAILED;
    }

    if ( __logger -> __pt < EZYLOG_PRIORITY_FATAL )
    {
        pthread_mutex_unlock( &( __logger -> __mutex ) );
        return EL_PT_BELOW_SET;
    }

    long msg_to_write_len = strlen( __msg ) + NOTMSG_IN_LOG_MAX_LENGTH;
    char* msg_to_write = ( char* ) malloc( msg_to_write_len );
    parse_layout( msg_to_write , __logger -> __layout , get_curtime( 0 ) , __logger -> __name , __msg , EZYLOG_PRIORITY_FATAL , NULL );
    strcat( msg_to_write , "\n" );
    fputs( msg_to_write , __logger -> __f );
    free( msg_to_write );
    pthread_mutex_unlock( &( __logger -> __mutex ) );
    return 0;
}

/**
 * @brief reset layout of a logger
 * 
 * @param __logger ezylog logger
 * @param __newlayout new layout need to be set
*/
void ezylog_chglayout( ezylog_logger_t* __logger , const char* __newlayout ){
    strcpy( __logger -> __layout , __newlayout );
    return;
}

/**
 * @brief reset priority of a logger
 * 
 * @param __logger ezylog logger
 * @param __newpt new priority need to be set
*/
void ezylog_chgpriority( ezylog_logger_t* __logger , ezylog_priority_t __newpt ){
    __logger -> __pt = __newpt;
    return;
}

/**
 * @brief logger close
 * 
 * @param __logger ezylog logger
*/
void ezylog_close( ezylog_logger_t* __logger ){
    fclose( __logger -> __f );
    free( __logger );
    return;
}