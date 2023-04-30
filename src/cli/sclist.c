#include"cli/sclist.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/tokens",
    "/save",
    "/timeout",
    "/model",
    "/version",
    "/usage",
    "/help",
    "/exit",
    NULL
};