CC= gcc
CFLAGS= -Wall -L./libmptcp
BINARY= mptcp

default:
	gcc -g -c -Ilibmptcp print_pkt.c mptcp_core.h mptcp_core.c mptcp_client.h mptcp_client.c mptcp_driver.c
	gcc -pthread -o mptcp mptcp_driver.o mptcp_client.o mptcp_core.o print_pkt.o -L. -lmptcp


.o.c:
    

clean:
	-rm -f *.o
	-rm -f mptcp