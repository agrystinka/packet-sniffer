#ifndef CMD_HANDLER
#define CMD_HANDLER

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

#define ADDRESS "socket"
void cmdhandler();
#endif
