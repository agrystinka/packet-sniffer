#include "main.h"
#include "logerr.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

int err_handle(const char *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    if (NULL != errmsg) {
        fprintf(stderr, "%sError:%s ", RED, RESET);
        vfprintf(stderr, errmsg, va);
        fprintf(stderr, "\n");
    }
    va_end(va);
    exit(EXIT_FAILURE);
}

int err_catch(const char *errmsg, ...)
{
    FILE *errlog;
    va_list va;
    va_start(va, errmsg);
    if (NULL != errmsg) {
        errlog = fopen("err.txt", "w");
        fprintf(errlog, "Error:\n");
        vfprintf(errlog, errmsg, va);
        fprintf(errlog, "\n");
        fclose(errlog);
    }
    va_end(va);
    exit(EXIT_FAILURE);
}
