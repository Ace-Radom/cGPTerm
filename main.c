#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>

#include"rwcfg.h"
#include"argparse.h"
#include"utils.h"
#include"ezylog.h"

int main( int argc , char** argv ){
    usrhome = getenv( "HOME" );
    // get usr home absolute path
    cfgdir = ( char* ) malloc( strlen( usrhome ) + strlen( "/.cgpterm" ) + 1 );
    sprintf( cfgdir , "%s%s" , usrhome , "/.cgpterm" );
    struct stat st = { 0 };
    if ( stat( cfgdir , &st ) == -1 )
    {
        if ( mkdir( cfgdir , 0700 ) == -1 )
        {
            perror( "Make Config DIR failed" );
            return 1;
        } // try make cfgdir; when failed, stop
    } // cfgdir doesn't exist
    // make cfgdir absolute path str, check if cfgdir exists; if not, make it

    cfginipath = ( char* ) malloc( strlen( cfgdir ) + strlen( "/config.ini" ) + 1 );
    sprintf( cfginipath , "%s%s" , cfgdir , "/config.ini" );
    if ( access( cfginipath , F_OK ) == -1 )
    {
        if ( create_config( cfginipath ) == -1 )
        {
            return 1;
        } // try make config.ini; when failed, stop
    } // config.ini not exists
    rconfig( cfginipath );
    // make config.ini absolute path str, check if config.ini exists; if not, make it and dump into a default config data

    logpath = ( char* ) malloc( strlen( cfgdir ) + strlen( "/chat.log" ) + 1 );
    sprintf( logpath , "%s%s" , cfgdir , "/chat.log" );
    logger = ezylog_init( "chat" , "[%T] %l: %P  %m" , logpath , EZYLOG_LOGMODE_APPEND , LOG_LEVEL );
    // make log file path; init logger and launch

    ezylog_logdebug( logger , "cGPTerm master process initialization complete, logger launch" );

    // ============================================================================================
    //                              End of cGPTerm master process init
    // ============================================================================================

    args_init();
    args_parse( argc , argv );
    if ( gota_help )
    {
        print_help();
        ezylog_logdebug( logger , "help printed" );
        goto stopmain;
    }
    args_actions_setcfg();
    cconfig();

stopmain:
    ezylog_logdebug( logger , "cGPTerm master process shutdown..." );
    ezylog_close( logger );
    return 0;
    
}
