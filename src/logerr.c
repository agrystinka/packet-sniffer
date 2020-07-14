#include "main.h"
#include "logerr.h"

/**
 * Catches errors from daemon.
 * @errmsg: pointer on error message.
 * @...: some additional arguments, that help to clarify type of error.
 *
 * Writes error message into log.txt file and close log.txt, dump.txt.
 * Finishes daemon proces with EXIT_FAILURE code.
 *
 * Return: EXIT_FAILURE.
 */
int err_catch(const char *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    fprintf(loging, "Error:\n");

    if (errmsg) {
        vfprintf(loging, errmsg, va);
        fprintf(loging, "\n");
    }
    va_end(va);
    fclose(loging);
    fclose(dump);
    exit(EXIT_FAILURE);
}
