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

#define DUMPFILE "dump.txt"
#define ADDRESS "socket"

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
                  *cmd_show   = "show",  //
                  *show_all   = "-a";    //


void command_show_a(void){
    FILE *show = fopen(DUMPFILE, "r");
    if (!show)
        err_handle("Cannot open %s. It might be using by another process.\nTry to use command STOP, firstly.\n", DUMPFILE);
    char c;

    /* Print each character in the file */
    while ((c = fgetc(show)) != EOF)
        printf("%c", c);

    fclose(show);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int cmd = 0;

    if (2 == argc) {
        if (!strcmp(argv[1], cmd_start)){
            cmd = 1;
        }
        else if (!strcmp(argv[1], cmd_stop)){
            cmd = 2;
        }
        else if (!strcmp(argv[1], cmd_reset)){
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
        if (!strcmp(argv[1], cmd_show)){
            if(!strcmp(argv[2], "-a"))
                command_show_a();
            // else if(!strcmp(argv[2], "-i"))
            //     command_show_a(argv[3]);
            else
                err_handle(USAGE, argv[0]);
        }
        else{
            err_handle(USAGE, argv[0]);
        }
    }
    else{
        err_handle(USAGE, argv[0]);
    }

    /*Create and open socket to comuticate with packet sniffer daemon*/
    int sock;
    struct sockaddr sa1 = {AF_UNIX, ADDRESS};

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == connect(sock, &sa1, sizeof(sa1) + sizeof(ADDRESS)))
        err_handle("Connection Failed \n", strerror(errno));

    write(sock, &cmd, sizeof(cmd));

    close(sock);
    return 0;
}
