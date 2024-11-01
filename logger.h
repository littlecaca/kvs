#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>


void stdio_log(const char *format, ...);
void stdio_error(const char *format, ...);

#ifndef NODEBUG
    #define LOG_DEBUG(format, ...) stdio_log("[DEBUG %s:%d %s] "\
        format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#else
    #define LOG_DEBUG(format, ...)
#endif

#define LOG_SYSERR(format, ...) stdio_error("[ERROR %s:%d %s] "\
    format "info: %s\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__, strerror(errno))

#define LOG_ERROR(format, ...) stdio_error("[ERROR %s:%d %s] "\
    format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
