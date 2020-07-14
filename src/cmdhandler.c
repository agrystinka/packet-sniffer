#include "cmdhandler.h"
#include "logerr.h"
#include "main.h"

/**
 * Implementation of command START.
 *
 * Make signal to sniffer to start writing the catch packets dump into dump file, by settint ACTIVE into 1.
 * If sniffer is already in ACTIVE writing mode, it dooes nothing.
 * Functions makes logs into log file and catchs errors, which makes writting into file impossible.
 *
 * Return: void.
 */
void cmd_start(void)
{
    if(!ACTIVE){
        ACTIVE = 1;
        dump = fopen(DUMPFILE,"a+");
        if(!dump)
           err_catch("Cannot open dump file 1.\n");
        _log(3, "Command START : done.\n");
    }
    else {
        _log(1, "Writing dump into file is already ACTIVE.\n");
    }
}

/**
 * Implementation of command STOP.
 *
 * Make signal to sniffer to stop writing the catch packets dump into dump file, by settint ACTIVE into 0.
 * If sniffer is already in DISACTIVE writing mode, it dooes nothing.
 * Functions makes logs into log file and catchs errors, which makes writting into file impossible.
 *
 * Return: void.
 */
void cmd_stop(void)
{
    if (ACTIVE) {
        ACTIVE = 0;
        fclose(dump);
        _log(3, "Command STOP : done.\n");
    }
    else {
        _log(1, "Writing dump into file is already DISACTIVE.\n");
    }
}

/**
 * Implementation of command RESET.
 *
 * Reset the dump file by deleting all information in it.
 *
 * Return: void.
 */
void cmd_reset(void)
{
    err_catch("RESET.\n");
}

/**
 * Handles commands got from cli by unix socket.
 *
 * This function is implementation of socket in server mode.
 * It gets command codes from cli by unix socket and serve it.
 * Also, it does signals for sniffer thread.

 * This functions write its logs in the log file.
 * If there are some problenms with socket, it catches errors.
 *
 * Possible commands are:
 * 1 - code of command START.
 * 2 - code of command STOP.
 * 3 - code of command RESET.
 *
 * Context: this function works seperatly from sniffer in another thread.
 *
 * Return: void.
 */
void cmdhandler(void)
{
    struct sockaddr sa0 = {AF_UNIX, ADDRESS};
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (-1 == sock)
        err_catch("Socket creation failed (%s)", strerror(errno));

    if (-1 == bind(sock, &sa0, sizeof(sa0) + sizeof(ADDRESS)))
        err_catch("Socket bind failed (%s)", strerror(errno));

    if (-1 == listen(sock, 1))
        err_catch("Socket listen failed. (%s)", strerror(errno));

    int cli;
    while(1) {
        cli = accept(sock, NULL, NULL);
        if (-1 == cli)
            err_catch("Cannot connect to CLI.", strerror(errno));
        _log(3, "Socket-in-server-mode connected.\n"); //log

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
            default:
                _log(1, "Socket-in-server-mode got INVALID COMMAND.\n");
        }
        close(cli);
    }
    unlink(ADDRESS);
}
