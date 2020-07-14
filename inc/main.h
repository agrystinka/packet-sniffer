#ifndef MAIN
#define MAIN

#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOGFILE "log.txt"
#define DUMPFILE "dump.txt"
#define IDFILE "daemon_id.txt"

extern FILE *dump;
extern FILE *loging;
extern int ACTIVE; //Flag if deamon is writing into dump file.

#endif
