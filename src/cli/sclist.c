#include"cli/sclist.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/tokens",
    "/usage",
    "/timeout",
    "/model",
    "/save",
    "/undo",
    "/last",
    "/version",
    "/help",
    "/exit",
    NULL
};