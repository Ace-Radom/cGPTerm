#include"cli/sclist.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/tokens",
    "/usage",
    "/save",
    "/timeout",
    "/model",
    "/version",
    "/help",
    "/exit",
    NULL
};