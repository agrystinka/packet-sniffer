#ifndef LOGERR
#define LOGERR

#include "main.h"

#define LVL 3

#define RESET   "\033[0m"       //no color
#define RED     "\033[1;31m"    //Errors

/**
 * Macros that implements daemon loging.
 * @lvl: type of log.
 * @msg: log message.
 *
 * It writes 3 types of logs:
 *   * INFO - short loging of main daemon actions.
 *   * SNIFFER logs - messages about catched packets.
 *   * DEBUG - more detailed logging of daemon actions.
 *
 * Return: void.
 */
#define _log(lvl, msg) do {                        \
    if (LVL >= lvl && NULL !=msg)                       \
        if(lvl == 1)                                    \
            fprintf(loging, ">>    INFO: %s", msg);     \
        else if (lvl == 2)                              \
            fprintf(loging, ">> SNIFFER: %s", msg);     \
        else if (lvl == 3)                              \
            fprintf(loging, ">>   DEBUG: %s", msg);     \
    } while(0)

int err_handle(const char *errmsg, ...);
int err_catch(const char *errmsg, ...);

#endif
