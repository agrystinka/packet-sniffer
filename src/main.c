#include "main.h"
#include "cmdhandler.h"
#include "sniffer.h"
#include "logerr.h"

FILE *dump = NULL;
FILE *loging = NULL;
int ACTIVE = 0;


void sig_term_handler(int signum)
{
    _log(1, "SIGTERM received.\n");
    fclose(loging);
    fclose(dump);
}

void catch_sigterm()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_term_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &_sigact, NULL);
}

/**
 * Runs core functions of daemon process.
 *
 * Creates two threads:
 * First thread works in socket-in-server-mode and handles comands from CLI.
 * Second thread is packet sniffer.
 *
 * Return: void.
 */
void daemon_core(void)
{
    _log(1, "Daemon sniffer run.\n");
    pthread_t *threads = alloca(2 * sizeof *threads);

    catch_sigterm();

    int f_sinsm = pthread_create(&threads[0], NULL, &cmdhandler, NULL);
    _log(3, "Socket-in-server-mode thread started.\n");

    int f_sn= pthread_create(&threads[1], NULL, &sn_start, NULL);
    _log(3, "Sniffer thread started.\n");

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    _log(3, "Threads finished work.\n");
}


/**
 * Creates deamon process.
 *
 * Creates daemon by creating child process and killing parent one.
 * Close stderr, stdin, stdout for this process.
 * Writes pid of daemon process into file.
 * It can be used to kill deamon process quickly without search it in process list.
 *
 * Return: void.
 */
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
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    umask(0);

    /* Open logfile in current directory */
    chdir(".");

    /*Close stdin, stdout and stderr*/
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    fprintf(loging, " Daemon process: %d\n", sid);

    FILE *d_id = fopen(IDFILE, "w+");
    if(!d_id)
        _log(1, "Did not saved daemon id into file.\n");
    fprintf(d_id, "%d\n", sid);
    fclose(d_id);
}

int main(void)
{
    loging = fopen(LOGFILE, "w");

    deamon_create();

    while (1) {
        fprintf(loging, " Daemon started.\n");
        daemon_core();
        break;
    }

    fprintf(loging, " Daemon terminated.\n");
    printf("Daemon terminated.\n");
    fclose(loging);
    return 0;
}
