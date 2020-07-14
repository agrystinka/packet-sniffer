#ifndef LOGERR
#define LOGERR

#define LVL 3

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
#define _log(lvl, msg) do {                             \
    if (LVL >= lvl && NULL !=msg) {                     \
        if(lvl == 1)                                    \
            fprintf(loging, ">>    INFO: %s", msg);     \
        else if (lvl == 2)                              \
            fprintf(loging, ">> SNIFFER: %s", msg);     \
        else if (lvl == 3)                              \
            fprintf(loging, ">>   DEBUG: %s", msg);     \
    }                                                   \
} while(0)

int err_catch(const char *errmsg, ...);

#endif
