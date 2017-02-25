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

    printf("charnum is: %s\n", charNum);
    char *interfaceRequest = concat("MPREQ ", charNum);
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