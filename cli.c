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
    "*  -a argument if provided results in verbose output with measured"
    " exec time.\n"
    "*  -t argument if provided specifies number of threads (%lld thread%s is"
    " used by default). If given as -t 0, the number of threads will"
    " be automatically picked as the number of CPUs available on the"
    " system, as reported by nproc.\n"
    "*  -n argument if provided specifies number of integration steps"
    " (%lld steps is used by default)\n"
    "*  start and stop arguments are required positionals specifing the"
    " integration interval as [start; stop]\n"
);

static const char *cmd_start  = "start",
                  *cmd_stop   = "stop",
                  *cmd_help   = "--help",
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
            printf("SHOW\n");
            cmd = 1;
            // domain or ip parse
        }
        else if (!strcmp(argv[1], cmd_stat)){
            printf("STAT\n");
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
    //socklen_t sa1_len;
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
