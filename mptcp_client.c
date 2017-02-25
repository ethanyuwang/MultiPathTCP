#include "mptcp_client.h"

char *buffer;
int total_bytes, global_ack_num, previous_global_ack_num, previous_ack_num;
int threadNum, packet_sent, packet_received;

void start_client(){
	printf("Starting client..\n");
    global_ack_num = 1;
    previous_global_ack_num = 1;
    previous_ack_num = 1;
    threadNum = 1;
    packet_sent = 0;
    packet_received = 0;

    /*-----------------------------load textfile for sending-----------------------------*/
    FILE *fp;
    long lSize;
    

    fp = fopen ( opt.filename , "rb" );
    if(!fp) {
        perror(opt.filename),exit(1);
    }

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );

    rewind( fp );

    /* allocate memory for entire content */
    buffer = calloc( 1, lSize+1 );
    if( !buffer ){
        fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    }

    /* copy the file into the buffer */
    if( 1!=fread( buffer , lSize, 1 , fp)) {
        fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);
    }

    /* prepare data */
    total_bytes = strlen(buffer);
    printf("File total size is: %d\n", total_bytes);
    //char interfaceRequest[strlen(interfaceRequestPtr)];
    //strcpy(interfaceRequest, interfaceRequestPtr);
    //total_bytes=sizeof(interfaceRequest);



    /*---------------initialize a socket as MPTCP to request interfaces------------------*/
    int sockfd = 0;
    int len = sizeof(struct sockaddr);  /* length of a sockaddr */
    struct hostent *hp;                 /* host information */
    struct sockaddr_in dest_addr;       /* remote destination address */
    struct sockaddr_in src_addr;        /* local sender address */


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



    /*---------------Request interfaces------------------*/
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
    char charNum[2];
    snprintf(charNum, 2, "%d\n", opt.num_interfaces);
    char *interfaceRequestPtr = concat("MPREQ ", charNum);

    /* convert char pointer to char array */
    char interfaceRequest[strlen(interfaceRequestPtr)];
    strcpy(interfaceRequest, interfaceRequestPtr);
    RequestHeader.total_bytes=sizeof(interfaceRequest+1);

    printf("total_bytes: %d\n", RequestHeader.total_bytes);
    printf("interfaceRequest: %s\n", interfaceRequest);

    struct packet RequestPacket;
    RequestPacket.header=&RequestHeader;
    RequestPacket.data=interfaceRequest;

    ssize_t size;
    print_pkt(&RequestPacket,0);
    if(size = mp_send(sockfd, &RequestPacket, RequestHeader.total_bytes, 0) < 0) {
        internalError("send interface request failed"); 
    }
    printf("size mp_sent: %lu\n", (unsigned long int)size);

    /* get available interfaces */
    struct mptcp_header ReplyHeader;
    char receiveData[RWIN];
    struct packet ReceivePacket;
    ReceivePacket.header=&ReplyHeader;
    ReceivePacket.data=receiveData;
    if( mp_recv(sockfd, &ReceivePacket, sizeof(receiveData), 0) < 0) {
        internalError("receive interface reply failed"); 
    }
    print_pkt(&ReceivePacket,1);

    /* parse reply into port numbers */
    char *str = ReceivePacket.data;
    chopPrefix(str);
    const char s[2] = ":";
    char *token;
    /* get the first token */
    token = strtok(str, s);



    /*----------------------walk through tokens and initialize clients---------------------*/
    pthread_t tid;

    while( token != NULL ) {
        int currentPortNum = (int) strtol(token, (char **)NULL, 10);

        pthread_create(&tid, NULL, clientThread, (void *)currentPortNum);

        token = strtok(NULL, s);
    }

    pthread_exit(NULL);



    close(sockfd);

    fclose(fp);
    free(buffer);

    endProgram("Connection closed");
}

/* subflow client socket thread */
void *clientThread(void *vargp)
{
    int interfacePort = (int)vargp;
    int thisThreadNum = threadNum;
    threadNum++;
 
    /*---------------initialize a subflow socket as MPTCP to send data ------------------*/
    int sockfd = 0;
    int len = sizeof(struct sockaddr);  /* length of a sockaddr */
    struct hostent *hp;                 /* host information */
    struct sockaddr_in dest_addr;       /* remote destination address */
    struct sockaddr_in src_addr;        /* local sender address */


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
    dest_addr.sin_port = htons(interfacePort);

    /* connect to server */
    if (mp_connect(sockfd , (struct sockaddr *)&dest_addr , sizeof(dest_addr)) < 0) {
        endProgram("Connect Failed");
    }
    else {
        printf( "connected to: %d\n", interfacePort );
    }


    /*--------------------send data-----------------------------*/
    while (global_ack_num>0)
    {
        /* take a piece of data; resend lost package*/
        char dataPiece[MSS];
        strncpy(dataPiece, buffer, MSS);
        buffer += MSS;
        dataPiece[MSS] = 0;
        packet_sent++;
        if (packet_received<packet_sent) {
            for (int i=0; i < (packet_sent-packet_received); i++)
                buffer -= MSS;
            packet_sent = packet_received;
        }

        /* construct local sender address */
        bzero((char *) &src_addr, sizeof(src_addr));
        //src_addr.sin_family = AF_INET;
        getsockname(sockfd, (struct sockaddr *) &src_addr, &len);

        /* request for available interfaces */
        struct mptcp_header dataHeader;
        dataHeader.dest_addr = dest_addr;
        dataHeader.src_addr = src_addr;
        dataHeader.seq_num = global_ack_num;
        dataHeader.ack_num = 0;
        dataHeader.total_bytes=total_bytes;

        struct packet dataPacket;
        dataPacket.header=&dataHeader;
        dataPacket.data=dataPiece;

        printf("==============Thread %d:==============\n", thisThreadNum);
        print_pkt(&dataPacket,0);
        if(mp_send(sockfd, &dataPacket, sizeof(dataPiece), 0) < 0) {
            internalError("send data packet failed"); 
        }

        /* get reply ack */
        struct mptcp_header ReplyHeader;
        char receiveData[RWIN];
        struct packet ReceivePacket;
        ReceivePacket.header=&ReplyHeader;
        ReceivePacket.data=receiveData;
        if( mp_recv(sockfd, &ReceivePacket, sizeof(receiveData), 0) < 0) {
            internalError("receive ack reply failed"); 
        }
        printf("==============Thread %d:==============\n", thisThreadNum);
        print_pkt(&ReceivePacket,1);

        /* update and synchronize q */
        global_ack_num = ReceivePacket.header->ack_num;


        if (ReceivePacket.header->ack_num/MSS>packet_received) {
            packet_received = ReceivePacket.header->ack_num/MSS;
        }
        printf("Packet recived: %d\n", packet_received);


        if (global_ack_num>previous_global_ack_num) {
            previous_global_ack_num = global_ack_num;
            //buffer += MSS;
        }
    }


    close(sockfd);

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

void chopPrefix(char *str)
{  
    //assert(n != 0 && str != 0);
    int n=5;
    size_t len = strlen(str);
    //if (n > len)
    //    return;  // Or: n = len;
    memmove(str, str+n, len - n + 1);
}
