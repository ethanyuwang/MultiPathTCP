#include "snc_client.h"


void start_client(){
	printf("client is running\n");

    int sockfd = 0;
    int serverlen;
    int recvlen;
    struct hostent *hp;     /* host information */
    struct sockaddr_in serv_addr;

    char recvBuff[BUFFSIZE], sendBuff[BUFFSIZE];

    fd_set rset;
    int maxfdp1;
    int bytes;

    /* initialize a socket as UDP or TCP */
    if(opt.udp) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    }
    else {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
    if(sockfd == -1){
        internalError("Socket failed to initialize");
        endProgram("Socket failed to initialize");
    }

    hp = gethostbyname(opt.hostname);
        if (!hp) {
            fprintf(stderr, "could not obtain address of %s\n", opt.hostname);
            //return 0;
        }

    //memset(recvBuff, '0', sizeof(recvBuff));
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    
    //if (opt.udp) {
        //memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
        bcopy((char *)hp->h_addr, (char *)&serv_addr.sin_addr.s_addr, hp->h_length);

    //}
    //else {
    if (0) {
        /* if hostname is provided use it */  
        if (opt.hostname!=NULL) {
            if (inet_pton(AF_INET, opt.hostname, &serv_addr.sin_addr)<0) {
                internalError("inet_pton for hostname failed");
            }
        }
        /* if source is provided use it */   
        else if (opt.ip!=NULL) {
            if (inet_pton(AF_INET, opt.ip, &serv_addr.sin_addr)<0) {
                internalError("inet_pton source failed");
            }
            //serv_addr.sin_addr.s_addr = inet_addr(opt.hostname);
        }
        /* defualt case */ 
        else {
            serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
    }
    serv_addr.sin_port = htons(opt.port);

    if(!opt.udp){
        if (connect(sockfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0) {
            internalError("Connect Failed");
        }
    }

    while(1)
    {
        /*recvlen = recvfrom(sockfd, recvBuff, BUFFSIZE, 0, (struct sockaddr *)&serv_addr, &serverlen);
        if (recvlen > 0) {
                printf("%s", recvBuff);
        }
        fgets(sendBuff, BUFFSIZE, stdin);

        if (sendto(sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&serv_addr, serverlen)<0) {
            internalError("Socket failed to send");
        }*/
        if (opt.udp) {
            bzero(sendBuff, BUFFSIZE);
            fgets(sendBuff, BUFFSIZE, stdin);
             
            //send the message
            serverlen = sizeof(serv_addr);
            if (sendto(sockfd, sendBuff, strlen(sendBuff) , 0 , (struct sockaddr *)&serv_addr, serverlen)<0) {
                internalError("sendto() failed");
            }

            /* print the server's reply */
            //bzero(recvBuff, BUFFSIZE);
            //bzero(sendBuff, BUFFSIZE);
            if(recvfrom(sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&serv_addr, &serverlen)<0){internalError("sendto() failed");
                internalError("recvfrom() failed");
            }
            printf("%s", sendBuff);
        }
        else {
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
        }
    }

    close(sockfd);
    endProgram("Connection closed");
}