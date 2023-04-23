#ifndef _ELENUM_H_
#define _ELENUM_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EZYLOG_PRIORITY_DEBUG = 3,
    EZYLOG_PRIORITY_INFO  = 2,
    EZYLOG_PRIORITY_ERROR = 1,
    EZYLOG_PRIORITY_FATAL = 0
} ezylog_priority_t;

typedef enum {
    EZYLOG_LOGMODE_APPEND,
    EZYLOG_LOGMODE_NEW
} ezylog_logmode_t;

#ifdef __cplusplus
}
#endif

#endif