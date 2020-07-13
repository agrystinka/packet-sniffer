#include "logerr.h"
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DUMPFILE "dump.txt"
#define ADDRESS "socket"
#define MAXLINE 30

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
    "*  show -i [ip] - argument is used to show number of received packets from [ip].\n\n"
);

static const char *cmd_start  = "start",
                  *cmd_stop   = "stop",
                  *cmd_reset  = "reset",
                  *cmd_help   = "--help",
                  *cmd_show   = "show",
                  *show_all   = "-a",
                  *show_ip    = "-i";

FILE *show = NULL;

static void command_show_all(void);
static void command_show_ip(const char *str_ip);

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
    else if (3 == argc &&
             !strcmp(argv[1], cmd_show) && //check if command is "show"
             !strcmp(argv[2], show_all)) { //check if command agr is "-a"
                command_show_all();
                return 0;
    }
    else if (4 == argc &&
             !strcmp(argv[1], cmd_show) && //check if command is "show"
             !strcmp(argv[2], show_ip)) {  //check if command agr is "-i"
                struct in_addr *ip;
                if(!inet_aton(argv[3], ip))  //check if asked IP is valid
                   err_handle("Invalid IP address.\n");
                command_show_ip(argv[3]);
                return 0;
    }
    else {
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

static void command_show_all(void)
{
    show = fopen(DUMPFILE, "r");
    if (!show)
        err_handle("Cannot open %s. It might be using by another process.\nTry to use command STOP, firstly.\n", DUMPFILE);
    char c;

    /* Print each character in the file */
    while ((c = fgetc(show)) != EOF)
        printf("%c", c);

    fclose(show);
    printf("\n");
}

static int get_line (char* string)
{
    char c;
    int i = 0;

while(i < MAXLINE - 1 && (c = getc(show)) != EOF && c != '\n')
        string[i++] = c;

    if (c == '\n')
           string[i++] = c;

    string[i] = '\0';
    return i;
}

static void command_show_ip(const char *str_ip)
{
    show = fopen(DUMPFILE, "r");
    if (!show)
        err_handle("Cannot open %s. It might be using by another process.\nTry to use command STOP, firstly.\n", DUMPFILE);

    char line[MAXLINE];
    int found = 0;

    int from = 0;
    int to = 0;

    while (get_line(line) > 0)
        if (strstr(line, str_ip) != NULL) {
            if (strstr(line, "SRC IP") != NULL)
                from++;
            else if (strstr(line, "DST IP") != NULL)
                to++;
            found++;
        }

    printf("Found %d packets:\n", found);
    printf("From %s:  %d\n", str_ip, from);
    printf("To %s:    %d\n", str_ip, to);

    fclose(show);
}
