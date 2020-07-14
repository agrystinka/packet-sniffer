#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DUMPFILE "dump.txt"
#define ADDRESS "socket"
#define MAXLINE 30

#define RESET   "\033[0m"       //no color
#define RED     "\033[1;31m"    //Errors

static const char *const USAGE = (
    "Invalid arguments.\n"
    "Try '%s --help' to get more information\n"
);

static const char *const HELP = (
    "*  Use %s with folowing arguments to interact with daemon packet-sniffer.\n\n"
    "*  start - argument is used to start writing sesion (write sniffed packet information into %s).\n"
    "*  stop - argument is used to finish writing sesion (stop write sniffed packet information into %2$s).\n"
    "*  reset - argument is used to finish writting session and clean %2$s.\n"
    "*  show -a - argument is used to show %2$s.\n"
    "*  show -i ${ip} - argument is used to show number of received packets from ${ip}, written in %2$s.\n\n"
    "*  Before using commands %1$s show* make sure that you closed writing session by using command %1$s stop.\n\n"
    "*  Work with packet-sniffer requires root privileges.\n\n"
);

static const char *cmd_start  = "start",
                  *cmd_stop   = "stop",
                  *cmd_reset  = "reset",
                  *cmd_help   = "--help",
                  *cmd_show   = "show",
                  *show_all   = "-a",
                  *show_ip    = "-i";

FILE *show = NULL;

static int err_handle(const char *errmsg, ...);
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
            printf(HELP, argv[0], DUMPFILE);
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

    if (-1 == sock)
        err_handle("Socket creation failed (%s)", strerror(errno));

    if (-1 == connect(sock, &sa1, sizeof(sa1) + sizeof(ADDRESS)))
        err_handle("Connection failed. %s\n", strerror(errno));

    if (-1 == write(sock, &cmd, sizeof(cmd)))
        err_handle("Write failed. %s\n", strerror(errno));

    close(sock);
    return 0;
}

/**
 * Handle errors from CLI.
 * @errmsg: pointer on error message.
 * @...: some additional arguments, that help to clarify type of error.
 *
 * Show error messages in stderr while using CLI and finish CLI process with EXIT_FAILURE code.
 *
 * Return: EXIT_FAILURE.
 */
static int err_handle(const char *errmsg, ...)
{
    va_list va;
    va_start(va, errmsg);
    if (errmsg) {
        fprintf(stderr, "%sError:%s ", RED, RESET);
        vfprintf(stderr, errmsg, va);
        fprintf(stderr, "\n");
    }
    va_end(va);
    exit(EXIT_FAILURE);
}

/**
 * Show packets written into dump.txt.
 *
 * Return: void.
 */
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

/**
 * Show number of packets send and received from some IP adress.
 * @str_ip: IP adress in the human readable format.
 *
 * Show info only about packets written in dump.txt.
 *
 * Return: void.
 */
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
