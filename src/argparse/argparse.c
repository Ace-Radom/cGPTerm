#include"argparse.h"

int args_actions_setcfg(){
    int action_num = 0;

    if ( gota_set_apikey )
    {
        action_num++;
        setcfg_OPENAI_API_KEY( gav_set_apikey );
    }
    if ( gota_set_timeout )
    {
        action_num++;
        setcfg_OPENAI_API_TIMEOUT( gav_set_timeout );
    }
    if ( gota_set_gentitle )
    {
        action_num++;
        setcfg_AUTO_GENERATE_TITLE();
    }
    if ( gota_set_saveperfix )
    {
        action_num++;
        setcfg_CHAT_SAVE_PERFIX( gav_set_saveperfix );
    }
    if ( gota_set_loglevel )
    {
        action_num++;
        setcfg_LOG_LEVEL( gav_set_loglevel );
    }

    if ( action_num != 0 )
        wconfig( CFG_DEFAULT_PATH );

    return action_num;
}