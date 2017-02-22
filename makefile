CC= gcc
CFLAGS= -Wall
BINARY= mptcp

#default:	mptcp_core.o mptcp_client.o mptcp_driver.o
default:	mptcp_core.o mptcp_driver.o
	$(CC) $(CFLAGS) -o $(BINARY) $^

.o.c:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o
	-rm -f mptcp