#ifndef MAIN
#define MAIN

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

#define LOGFILE "log.txt"
//#define WLOGFILE "writinglog.txt"
#define DUMPFILE "dump.txt"

extern FILE *dump;
extern FILE *loging;
extern FILE *wlog ;
extern int ACTIVE; //Flag if deamon is writing into dump file.


#endif
