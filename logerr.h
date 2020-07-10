#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

#define LVL 3

//extern FILE *loging;

/*log colors*/
#define RESET   "\033[0m"       //no color
#define RED     "\033[1;31m"    //Errors
#define BLUE    "\033[1;34m"    //Info
#define GRAY    "\033[1;37m"    //Sniffer log
#define GREEN   "\033[1;32m"    //Debug

#define _log(lvl, msg, ...) do {                                                         \
    if (LVL >= lvl && NULL !=msg)                                            \
        if(lvl == 1)                                                                     \
            fprintf(loging, "%s>> INFO:%s %s", BLUE, RESET, msg);                        \
        else if (lvl == 2)                                                               \
            fprintf(loging, "%s>> SNIFFER LOG:%s %s", GRAY, RESET, msg);                 \
        else if (lvl == 3)                                                               \
            fprintf(loging, "%s>> DEBUG:%s %s", GREEN, RESET, msg);                      \
    } while(0)

int err_handle(const char *errmsg, ...);
