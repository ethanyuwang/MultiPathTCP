#include "mptcp_client.h"

void start_client(){
	printf("Starting client..\n");

    int sockfd = 0;
    int serverlen;
    int recvlen;
    int len = sizeof(struct sockaddr);  /* length of a sockaddr */
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
    dest_addr.sin_port = htons(opt.port);

    /* connect to server */
    if (mp_connect(sockfd , (struct sockaddr *)&dest_addr , sizeof(dest_addr)) < 0) {
        endProgram("Connect Failed");
    }

    /* construct local sender address */
    bzero((char *) &src_addr, sizeof(src_addr));
    //src_addr.sin_family = AF_INET;
    getsockname(sockfd, (struct sockaddr *) &src_addr, &len);

    /* request for available interfaces */
    struct mptcp_header RequestHeader;
    RequestHeader.dest_addr = dest_addr;
    RequestHeader.src_addr = src_addr;
    RequestHeader.seq_num = 1;
    RequestHeader.ack_num = 0;

    /* convert int num_interfaces to char */
    char charNum[2];// = intToChar(opt.num_interfaces);
    //char buffer[2];
    snprintf(charNum, 2, "%d\n", opt.num_interfaces);

    printf("charnum is: %s\n", charNum);
    char *interfaceRequest = concat("MPREQ ", charNum);//intToChar(opt.num_interfaces));
    RequestHeader.total_bytes=sizeof(interfaceRequest);

    struct packet RequestPacket;
    RequestPacket.header=&RequestHeader;
    RequestPacket.data=interfaceRequest;

    int size;
    print_pkt(&RequestPacket);
    if(size = mp_send(sockfd, &RequestPacket, RequestHeader.total_bytes, 0) < 0) {
        internalError("send interface request failed"); 
    }
    printf("size mp_sent: %lu\n", (unsigned long int)size);

    struct mptcp_header ReplyHeader;
    char receiveData[100];
    struct packet ReceivePacket;
    ReceivePacket.header=&ReplyHeader;
    ReceivePacket.data=receiveData;
    if( mp_recv(sockfd, &ReceivePacket, sizeof(ReceivePacket), 0) < 0) {
        internalError("receive interface reply failed"); 
    }
    print_pkt(&ReceivePacket);



    close(sockfd);
    endProgram("Connection closed");
}

/* utility functions */
char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    fprintf("Concate: %s\n", result);
    return result;
}

char* intToChar(int num) {
    char buffer[2];
    snprintf(buffer, 2, "%d\n", num);
    return buffer;
}
