#include"utils.h"

ezylog_logger_t* logger = NULL;
ini_t* config = NULL;

char* OPENAI_API_KEY = NULL;
double OPENAI_API_TIMEOUT = 30.0;
bool AUTO_GENERATE_TITLE = true;
char* CHAT_SAVE_PERFIX = NULL;
ezylog_priority_t LOG_LEVEL = EZYLOG_PRIORITY_INFO;