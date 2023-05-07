#include"cli/sclist.h"

/**
 * @brief all available slash commands. they will be searched by rl_completion_slash_command_search
*/
const char* slash_commands[] = {
    "/raw",
    "/tokens",
    "/usage",
    "/timeout",
    "/model",
    "/rand",
    "/save",
    "/undo",
    "/last",
    "/copy",
    "/version",
    "/help",
    "/exit",
    NULL
};