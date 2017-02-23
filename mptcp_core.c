#include "mptcp_core.h"


struct options opt;

void options_init(void){
    opt.num_interfaces = 0;
}

void options_print(void){

    printf("Number of interfaces: %d ", opt.num_interfaces);

    if (opt.hostname != NULL)
        printf("Hostname: %s, ", opt.hostname);

    if (opt.ip != NULL)
        printf("IP: %s, ", opt.ip);

    printf("Port: %d ", opt.port);

    if (opt.filename != NULL)
        printf("Filename: %s, ", opt.filename);

    printf("\n");
}


int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;

    struct in_addr **addr_list;
    int i;
    if ( (he = gethostbyname( hostname ) ) == NULL) {
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip , inet_ntoa(*addr_list[i]));
        return 0;
    }
    return 1;
}


void internalError(const char *format, ...){
    fprintf(stderr, "internal error: ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}


void endProgram(const char *format, ...){
    fprintf(stderr, "Fatal error: ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(EXIT_FAILURE);
}