#include "cmdhandler.h"
#include "sniffer.h"
#include "logerr.h"

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

#define LOGFILE "log.txt"
#define DUMPFILE "dump.txt"

//FILE *dump = NULL;
FILE *loging = NULL;
int ACTIVE = 0;

void daemon_core(void)
{
    _log(1, "Daemon sniffer run.\n");
    //ACTIVE = 0;
    pthread_t *threads = alloca(2 * sizeof *threads);

    int f_sinsm = pthread_create(&threads[0], NULL, &cmdhandler, NULL);
    _log(3, "Socket-in-server-mode thread started.\n");

    int f_sn= pthread_create(&threads[1], NULL, &sn_start, NULL);
    _log(3, "Sniffer thread started.\n");

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    _log(3, "Threads finished work.\n");
}

void deamon_create(void)
{
    pid_t pid = 0;
    pid_t sid = 0;

    /* Fork the parent process */
    pid = fork();

    /* Indication of fork() failure*/
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    sid = setsid();
    if(sid < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    umask(0);

    /* Open logfile in current directory */
    chdir(".");

    /*Close stdin, stdout and stderr*/
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    loging = fopen(LOGFILE, "w");
    fprintf(loging, " Daemon process: %d\n", sid);
}

int main(void)
{
    deamon_create();

    while (1) {
        fprintf(loging, " Daemon started.\n");
        daemon_core();
        //sleep(40);
        break;
    }

    fprintf(loging, " Daemon terminated.\n");
    fclose(loging);
    return 0;
}
