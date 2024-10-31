#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>


void stdio_log(const char *format, ...);

#ifndef NODEBUG
    #define LOG_DEBUG(format, ...) stdio_log("[DEBUG %s:%d %s] "\
        format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_ERROR(format, ...) stdio_log("[ERROR %s:%d %s] "\
        format "%s\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__, strerror(errno))
#else
    #define LOG_DEBUG(format, ...)
#endif
