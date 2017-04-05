# simple_netcat

## Overview

As a widely used internet protocol, TCP connections are limited by the the bandwidth of the link. However we could overcome this limitation without upgrading hardwares. Essentially, when there are multiple interfaces available on both the server and the client, we connect them with many TCP  subflows to simultaneously send packets for a greater throughput. This is a multipath capable TCP client implemented in C. 

## Usage

To run the client, you should execute in the following format:
        
mptcp [ -n num_interfaces ] [ -h hostname ] [ -p port ] [ -f filename ]
        
        [ -n ] : number of connections ( paths ) you would like to have with the server
        [ -h ] : hostname of the server where to connect
        [ -p ] : port number on the server where to start the initial connection
        [ -f ] : name of the file to transfer to the server
