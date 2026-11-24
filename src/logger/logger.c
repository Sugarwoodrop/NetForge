#include "../../include/netforge/logger/logger.h"

void log_msg(log_level_t level, const char *file, int line, const char *fmt, ...){
    static const char *level_names[] = {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };

    fprintf(stderr, "[%s] %s:%d ", level_names[level], file, line);

    va_list args;
    va_start(args, fmt);
    
    vfprintf(stderr, fmt, args);

    va_end(args);

    fprintf(stderr, "\n");
}