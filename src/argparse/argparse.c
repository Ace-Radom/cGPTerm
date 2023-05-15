#include"argparse.h"

int args_actions_setcfg(){
    int action_num = 0;

    if ( gota_set_host )
    {
        action_num++;
        setcfg_OPENAI_HOST( gav_set_host );
        printf( "OPENAI_HOST set to '%s'\n" , gav_set_host );
        ezylog_logdebug( logger , "cfg -> OPENAI_API_HOST set to '%s'" , gav_set_host );
    }
    if ( gota_set_apikey )
    {
        action_num++;
        setcfg_OPENAI_API_KEY( gav_set_apikey );
        printf( "OPENAI_API_KEY set to '%s'\n" , gav_set_apikey );
        ezylog_logdebug( logger , "cfg -> OPENAI_API_KEY set to '%s'" , gav_set_apikey );
    }
    if ( gota_set_timeout )
    {
        action_num++;
        setcfg_OPENAI_API_TIMEOUT( gav_set_timeout );
        printf( "OPENAI_API_TIMEOUT set to %lf\n" , gav_set_timeout );
        ezylog_logdebug( logger , "cfg -> OPENAI_API_TIMEOUT set to %lf" , gav_set_timeout );
    }
    if ( gota_set_gentitle )
    {
        action_num++;
        setcfg_AUTO_GENERATE_TITLE();
        printf( "AUTO_GENERATE_TITLE set to %s\n" , ( AUTO_GENERATE_TITLE ) ? "false" : "true" );
        ezylog_logdebug( logger , "cfg -> AUTO_GENERATE_TITLE set to %s" , ( AUTO_GENERATE_TITLE ) ? "false" : "true" );
    }
    if ( gota_set_saveperfix )
    {
        action_num++;
        setcfg_CHAT_SAVE_PERFIX( gav_set_saveperfix );
        printf( "CHAT_SAVE_PERFIX set to '%s'\n" , gav_set_saveperfix );
        ezylog_logdebug( logger , "cfg -> CHAT_SAVE_PERFIX set to '%s'" , gav_set_saveperfix );
    }
    if ( gota_set_loglevel )
    {
        action_num++;
        char* new_loglevel = parse_priority_to_str( parse_str_to_priority( gav_set_loglevel ) );
        setcfg_LOG_LEVEL( new_loglevel );
        printf( "LOG_LEVEL set to '%s'\n" , new_loglevel );
        ezylog_logdebug( logger , "cfg -> LOG_LEVEL set to '%s'" , new_loglevel );
    }

    if ( action_num != 0 )
        if ( wconfig( cfginipath ) == -1 )
        {
            fprintf( stderr , "[args_actions_setcfg] -> open config.ini failed\n" );
            return -1;
        }

    return action_num;
}

void print_help(){
    arg_print_glossary( stdout , argtable , ARG_PRINT_GLOSSARY_FORMAT );
    return;
}