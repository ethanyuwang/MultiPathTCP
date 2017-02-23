#ifndef _MPTCP_CORE_H_
#define _MPTCP_CORE_H_

#include "mptcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netdb.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define BUFFSIZE 2048

/* struct that stores ip address, port, and options */
struct options{
    /* number of interfaces */
    int num_interfaces;
    /* source ip address */
    char *ip;
    /* hostname */
    char *hostname;
    /* Port number */
    unsigned short port;
    /* filename */
    char *filename;
};

/* global options struct */
extern struct options opt;

/* initialize options struct to default values */
extern void options_init(void);

/* print parsed command line */
extern void options_print(void);

/* resolve hostname to ip */
extern int hostname_to_ip(char * hostname , char* ip);

/* print error message to @stderr*/
extern void internalError(const char *format, ...);

/* print message to @stderr and exit */
extern void endProgram(const char *format, ...);

#endif
