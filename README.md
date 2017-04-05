# simple_netcat

## Overview

TCP is widely used in the Internet today. Most all traffic nowadays is sent over some variant of the TCP protocol. Connections are limited by the bandwidth of the link, but the fact that many different paths may exist between 2 endpoints could be leveraged to increase this bandwidth. Essentially, if a host has more than one interface associated with it, and the client and server are both mptcp capable, they could distribute the load across multiple flows. For instance, if you want to watch a video on YouTube from your phone, MPTCP could make it possible to connect to WiFi and 4G/LTE data simultaneously to reduce buffering time. In this assignment, you will be sending a file to a remote server across multiple subflow paths.

## Usage

The client ( you ) will specify a number of interfaces you want to request from the server located at a hostname and port number in the command line arguments. Your client program should execute in the following format:
        
mptcp [ -n num_interfaces ] [ -h hostname ] [ -p port ] [ -f filename ]
        
        [ -n ] : number of connections ( paths ) you would like to have with the server
        [ -h ] : hostname of the server where to connect
        [ -p ] : port number on the server where to start the initial connection
        [ -f ] : name of the file to transfer to the server
