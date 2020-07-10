#include "sniffer.h"
#include "logerr.h"
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <signal.h>

#define NAMEFILE "loging.txt"
#define ADDRESS "socket1"

sem_t start_sem;
int STOP = 0;
int ACTIVE = 0;
int F = 0;

FILE *dump;
FILE *loging;

// static void sniffer(){
//     //  sem_wait(&start_sem);  //wait until start_sem == 1
//     sn_start();
// }

static void socket_in_server_mode(void)
{
    struct sockaddr sa0 = {AF_UNIX, ADDRESS};
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == sock)
        err_handle("Socket creation failed (%s)", strerror(errno));

    if (-1 == bind(sock, &sa0, sizeof(sa0) + sizeof(ADDRESS)))
        err_handle("Socket bind failed (%s)", strerror(errno));

    listen(sock, 1);
    int cli;

    while(1) {
        cli = accept(sock, NULL, NULL);
        _log(1,"Server connected.\n"); //log

        int cmd;

        ssize_t n = read(cli, &cmd, sizeof(cmd));
        _log(3, "Got command from CLI.\n");

        switch (cmd) {
            case 1:
                _log(1, "Got command START.\n");
                if(ACTIVE == 0){
                    ACTIVE = 1;
                    sem_post(&start_sem);
                    dump = fopen("dump.txt","a+");
                    if(!dump)
                        err_handle("Cannot open dump file.\n");
                    _log(1, "Command START : done.");
                }
                else _log(1, "ALREADY ACTIVE\n");
                break;
            case 2:
                _log(1, "Got command STOP.\n");
                if(ACTIVE == 1){
                    ACTIVE = 0;
                    fclose(dump);
                    _log(1, "Command STOP : done.\n");
                    fclose(loging);
                }
                else _log(1, "ALREADY DISACTIVE\n");
                break;
            case 3:
                _log(1, "Got command SHOW.\n");
                break;
            case 4:
                _log(1, "Got command SELECT.\n");
                break;
            case 5:
                _log(1, "Got command STAT.\n");
                break;
            default:
                err_handle("Got invalid command.\n");
}
        close(cli);
        // delete socket file 2
    }
    close(sock);
    unlink(ADDRESS);
    pthread_exit(0);
}

int main()
{
    loging = fopen(NAMEFILE, "w");
    _log(3, "Server run.\n");

    sem_init(&start_sem, 0, 0);  //initialize blocked semophore
    pthread_t *threads = alloca(2 * sizeof *threads);

    int f_sinsm = pthread_create(&threads[0], NULL, &socket_in_server_mode, NULL);
    _log(3, "Socket-in-server-mode thread started.\n");

    int f_sn= pthread_create(&threads[1], NULL, &sn_start, NULL);
    _log(3, "Sniffer thread started.\n");

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    sem_destroy(&start_sem);
    _log(3, "Sniffer threads finished work.\n");

    fclose(loging);
    return 0;
}
