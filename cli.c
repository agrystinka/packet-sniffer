#include "logerr.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>

//  Change this names
#define ADDRESS "socket1"
#define MYADDRESS "socket2"

static const char *const USAGE = (
    "Invalid arguments.\n"
    "Try '%s --help' for more information\n"
);

static const char *const HELP = (
    " Demaon packet-sniffer is already run.\n"
    " Use folowing arguments to interact with it.\n\n"
    "*  start - argument is used to start writing sniffed packet information into dump.txt.\n"
    "*  stop - argument is used to stop writing sniffed packet information into dump.txt.\n"
    "*  reset - argument is used to clean dump.txt.\n"
    "*  show - argument is used to show dump.txt.\n"
    "*  show -i [ip] - argument is used to show received packets from [ip].\n"
    "*  show -d [domain] - argument is used to show received packets from [domain].\n\n"
);

static const char *cmd_start  = "start", //
                  *cmd_stop   = "stop",  //
                  *cmd_reset  = "reset", //
                  *cmd_help   = "--help",//
                  *cmd_select = "select",
                  *cmd_stat   = "stat",
                  *cmd_show   = "show";

int main(int argc, char *argv[])
{   //use goto to go to err_handle
    int cmd = 0;
    if (2 == argc) {
        if (!strcmp(argv[1], cmd_start)){
            printf("OK\n");
            cmd = 1;
        }
        else if (!strcmp(argv[1], cmd_stop)){
            printf("STOP\n");
            cmd = 2;
        }
        else if (!strcmp(argv[1], cmd_reset)){
            printf("RSET\n");
            cmd = 3;
        }
        else if (!strcmp(argv[1], cmd_help)){
            printf(HELP);
            return 0;
        }
        else{
            err_handle(USAGE, argv[0]);
        }
    }
    else if (3 == argc){
        printf("3 OK\n");
        if (!strcmp(argv[1], cmd_show)){
            //printf("SHOW\n");
            cmd = 1;
            // domain or ip parse
        }
        else if (!strcmp(argv[1], cmd_stat)){
            //printf("STAT\n");
            cmd = 2;
        }
        else{
            err_handle(USAGE, argv[0]);
        }
    }
    else{
        err_handle(USAGE, argv[0]);
    }

    int sock;
    struct sockaddr sa0 = {AF_UNIX, MYADDRESS};
    struct sockaddr sa1 = {AF_UNIX, ADDRESS};

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == bind(sock, &sa0, sizeof(sa0) + sizeof(MYADDRESS)))
            err_handle("Socket bind failed (%s)", strerror(errno));

    if (-1 == connect(sock, &sa1, sizeof(sa1) + sizeof(MYADDRESS)))
        err_handle("Connection Failed \n", strerror(errno));

    write(sock, &cmd, sizeof(cmd));

    close(sock);
    unlink(MYADDRESS);
    return 0;
}
