#include"argparse/args.h"

// --------------------------- normal args ---------------------------

struct arg_lit* arg_help = NULL;

// --------------------------- setcfg args ---------------------------

struct arg_str* arg_set_apikey = NULL;
struct arg_dbl* arg_set_timeout = NULL;
struct arg_lit* arg_set_gentitle = NULL;
struct arg_str* arg_set_saveperfix = NULL;
struct arg_str* arg_set_loglevel = NULL;

// ---------------------------- argtable2 ----------------------------

struct arg_end* end_arg = NULL;
void* argtable[] = { NULL , NULL , NULL , NULL , NULL , NULL , NULL };

// -------------------------- args function --------------------------

void args_init(){
    arg_help = arg_lit0( "h" , "help" , "Show Help Messages and exit" );

    arg_set_apikey     = arg_str0( NULL , "set-apikey"     , "<OpenAI API Key>" , "Set API Key for OpenAI"                                 );
    arg_set_timeout    = arg_dbl0( NULL , "set-timeout"    , "<API Timeout>"    , "Set maximum waiting time for API requests"              );
    arg_set_gentitle   = arg_lit0( NULL , "set-gentitle"   ,                      "Set whether to automatically generate a title for chat" );
    arg_set_saveperfix = arg_str0( NULL , "set-saveperfix" , "<Prefix>"         , "Set chat history file's save perfix"                    );
    arg_set_loglevel   = arg_str0( NULL , "set-loglevel"   , "<Log Level>"      , "Set log level: DEBUG, INFO, ERROR, FATAL"               );

    end_arg = arg_end( 20 );

    void* args[] = {
        arg_help,
        arg_set_apikey,
        arg_set_timeout,
        arg_set_gentitle,
        arg_set_saveperfix,
        arg_set_loglevel,
        end_arg
    };

    for ( int i = 0 ; i < sizeof( args ) / sizeof( args[0] ) ; i++ )
        argtable[i] = args[i];

    return;
}

int args_parse( int __argc , char** __argv ){
    if ( arg_nullcheck( argtable ) != 0 )
    {
        fprintf( stderr , "[args_parse] -> arg_nullcheck() didn't return 0\n" );
        return -1;
    }
    int nerrors;
    if ( ( nerrors = arg_parse( __argc , __argv , argtable ) ) != 0 )
    {
        arg_print_errors( stderr , end_arg , __argv[0] );
        arg_print_glossary( stdout , argtable , ARG_PRINT_GLOSSARY_FORMAT );
        // print errors and help page
        return nerrors;
    }
    return 0;
}

void args_free(){
    arg_freetable( argtable , sizeof( argtable ) / sizeof( argtable[0] ) );
    return;
}