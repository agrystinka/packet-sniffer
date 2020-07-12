#include "main.h"
#include "logerr.h"

/**
 * Handle errors from CLI.
 * @errmsg: pointer on error message.
 * @...: some additional arguments, that help to clarify type of error.
 *
 * Show error messages in stderr while using CLI and finish CLI process with EXIT_FAILURE code.
 *
 * Return: EXIT_FAILURE.
 */
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

/**
 * Catches errors from daemon.
 * @errmsg: pointer on error message.
 * @...: some additional arguments, that help to clarify type of error.
 *
 * Writes error message into err.txt file and finishes daemon proces with EXIT_FAILURE code.
 *
 * Return: EXIT_FAILURE.
 */
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
