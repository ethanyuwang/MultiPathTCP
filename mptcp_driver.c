#include "mptcp_core.h"
#include "mptcp_client.h"

//./mptcp -n 6 -h 128.111.68.197 -p 5176 -f lala

/* utility functions */
void inputError(const char *format, ...){
    fprintf(stderr, "invalid or missing options\n");
    fprintf(stderr, "usage: MPTCP [-l] [-u] [-s source_ip_address] [hostname] port\n");

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

int validate_num_interfaces(char *n){
    int num_interfaces = atoi(n);

    if(num_interfaces < 1){
        inputError("Invalid interface number: %s. Interface number has to be greater than 0\n", n);
    }

    if(num_interfaces > 16){
        inputError("Invalid interface number: %s. Interface number maximum is 16\n", n);
    }
    return num_interfaces;
}

int validate_port(char *p){
    int port = atoi(p);

    if(port < 1025 || port > 65535){
        inputError("Invalid port number: %s\n", p);
    }
    return port;
}

char* validate_filename(char *f){
    int fd = open(f, O_RDONLY);
    if (fd == -1)
    {
        inputError("Error opening file: %s\n", f);
    }
    return f;
}

/* parse input and execute */
int main(int argc, char *argv[]){
    /* initialize options */
    options_init();

    /* parse command line args */
    int c;
    //int argMax = argc;
    while ((c = getopt(argc, argv, "n:h:p:f:")) !=-1) {
        /* parse command line args */
        switch(c){
            case 'n': opt.num_interfaces = validate_num_interfaces(optarg);
                      break;

            case 'h': opt.hostname = optarg;
                      break;

            case 'p': opt.port = validate_port(optarg);
                      break;

            case 'f': opt.filename = validate_filename(optarg);
                      break;

            default: inputError(" "); //inputError();
        }
    }
    options_print();


    /* dispatch client */
    start_client();

    return 0;
}