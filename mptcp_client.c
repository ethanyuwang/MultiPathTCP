#include "mptcp_client.h"

void start_client(){
	printf("Starting client..\n");

    int sockfd = 0;
    int serverlen;
    int recvlen;
    struct hostent *hp;                 /* host information */
    struct sockaddr_in dest_addr;       /* remote destination address */
    struct sockaddr_in src_addr;        /* local sender address */

    char recvBuff[BUFFSIZE], sendBuff[BUFFSIZE];

    fd_set rset;
    int maxfdp1;
    int bytes;

    /* initialize a socket as MPTCP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd == -1){
        endProgram("Socket failed to initialize");
    }

    /* get host address */
    hp = gethostbyname(opt.hostname);
    if (!hp) {
        endProgram("Could not obtain IP address of %s", opt.hostname);
    }

    /* construct remote destination address */
    bzero((char *) &dest_addr, sizeof(dest_addr));

    dest_addr.sin_family = AF_INET;

    bcopy((char *)hp->h_addr, (char *)&dest_addr.sin_addr.s_addr, hp->h_length);
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_port = htons(opt.port);

    /* construct local sender address */
    bzero((char *) &src_addr, sizeof(src_addr));

    src_addr.sin_family = AF_INET;

    bcopy((char *)hp->h_addr, (char *)&src_addr.sin_addr.s_addr, hp->h_length);
    src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    src_addr.sin_port = htons(opt.port); //?????????????????


    /* connect to server */
    struct mptcp_header RequestHeader;// = malloc(sizeof(*RequestHeader));
    RequestHeader.dest_addr = dest_addr;
    RequestHeader.src_addr = src_addr;
    char *interfaceRequest = concat("MPREQ n_paths"+);

    struct packet RequestPacket;

    if (mp_connect(sockfd , (struct sockaddr *)&dest_addr , sizeof(dest_addr)) < 0) {
        endProgram("Connect Failed");
    }


    /* get available interface numbers */
    //endProgram("Conneted to mtpct server");

    /* request for available interfaces */

   /* while(1)
    {

            fgets(sendBuff, BUFFSIZE, stdin);
            if( send(sockfd , sendBuff , strlen(sendBuff) , 0) < 0) {
                internalError("send Failed"); 
            }

            if( recv(sockfd , recvBuff , 2000 , 0) < 0) {
                internalError("recv Failed");     
            }
            else {
                printf("%s", recvBuff);
            }
    } */

    //close(sockfd);
    //endProgram("Connection closed");
}

void requestPaths(){
    struct mptcp_header RequestHeader;// = malloc(sizeof(*RequestHeader));
    RequestHeader.dest_addr = dest_addr;

    struct packet RequestPacket;

}

/* utility functions */
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* int(const char *s1, const char *s2)
char snum[5];

// convert 123 to string [buf]
itoa(num, snum, 10);