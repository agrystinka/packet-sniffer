#include "cmdhandler.h"
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


// #define DUMPFILE "dump.txt"
// extern int ACTIVE;
// FILE *dump;

void cmd_start(void)
{
    if(ACTIVE == 0){
        printf("START.\n");
        ACTIVE = 1;
        dump = fopen(DUMPFILE,"a+");
        if(!dump)
           err_catch("Cannot open dump file 1.\n");
        _log(3, "Command START : done.");
    }
    else _log(1, "Writing dump into file is already ACTIVE.\n");
}

void cmd_stop(void)
{
    if (ACTIVE == 1) {
        printf("STOP.\n");
        fclose(dump);
        fclose(loging);
        _log(3, "Command STOP : done.\n");
    }
    else _log(1, "Writing dump into file already DISACTIVE.\n");
}

void cmd_reset(void)
{
    if(dump != NULL)
        fclose(dump);
    dump = fopen("dump.txt","w");
    fclose(dump);
}

void cmdhandler()
{
    struct sockaddr sa0 = {AF_UNIX, ADDRESS};
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == sock)
        err_catch("Socket creation failed (%s)", strerror(errno));

    if (-1 == bind(sock, &sa0, sizeof(sa0) + sizeof(ADDRESS)))
        err_catch("Socket bind failed (%s)", strerror(errno));

    listen(sock, 1);
    int cli;

    while(1) {
        cli = accept(sock, NULL, NULL);
        _log(1,"Socket-in-server-mode connected.\n"); //log

        int cmd;

        ssize_t n = read(cli, &cmd, sizeof(cmd));
        _log(3, "Socket-in-server-mode got command from CLI.\n");

        switch (cmd) {
            case 1:
                _log(1, "Socket-in-server-mode got command START.\n");
                cmd_start();
                break;
            case 2:
                _log(1, "Socket-in-server-mode got command STOP.\n");
                cmd_stop();
                break;
            case 3:
                _log(1, "Socket-in-server-mode got command RESET.\n");
                cmd_reset();
                break;
            case 4:
                _log(1, "Got command SELECT.\n");  //change
                break;
            case 5:
                _log(1, "Got command STAT.\n"); //change
                break;
            default:
                _log(1, "Socket-in-server-mode got INVALID COMMAND.\n");  //or err_catch ?
        }
        close(cli);
        // delete socket file 2
    }
}
