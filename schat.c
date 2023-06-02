// Jrp9pte
// Jay Patel

//#include <stropts.h>		Might need according to man page
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>


int main( int argc, char* argv[]   ) {

	if (argc <= 2){

		// start by getting a random port from the ephemeral port range
		srandom(getpid()); // random seed based on this process's OS-assigned ID
    		int port = 0xc000 | (random()&0x3fff); // random element of 49152–65535
		if( argc == 2){
			port = atoi( argv[1]);
		}
    		// create an address structure: IPv4 protocol, any IP address, on given port
    		// note: htonl and htons are endian converters, essential for Internet communication
    		struct sockaddr_in ipOfServer;
    		memset(&ipOfServer, 0, sizeof(struct sockaddr_in));
    		ipOfServer.sin_family = AF_INET;
    		ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);
    		ipOfServer.sin_port = htons(port);

	    	// we'll have one socket that waits for other sockets to connect to it
    		// those other sockets will be the ones we used to communicate
    		int listener = socket(AF_INET, SOCK_STREAM, 0);

    		// and we need to tell the OS that this socket will use the address created for it
    		bind(listener, (struct sockaddr*)&ipOfServer , sizeof(ipOfServer));

    		// wait for connections; if too many at once, suggest the OS queue up 20
    		listen(listener , 20);

    		system("host $HOSTNAME"); // display all this computer's IP addresses
    		printf("The server is now listening on port %d\n", port); // and listening port


		printf("Waiting for a connection\n");
        	// get a connection socket (this call will wait for one to connect)
        	int connection = accept(listener, (struct sockaddr*)NULL, NULL);
        	close(listener); // Get Connection and close listener as its not needed anymore
		char* buff[5000]; // Buffer for inputs and  transferring data from socket to stdin

		//Using poll to handle reading and writing to socket
		struct pollfd fds[2];
                int timeout_msecs = 60000;
                fds[0].fd = connection;
                fds[1].fd = 0;
                fds[0].events = POLLIN;
                fds[1].events = POLLIN;
                int ret;
                int i;
//              Handling reading and writing to socket
                while(1){
                      	ret = poll(fds, 2, timeout_msecs);
	                if( ret >= 1){ //  if poll was succesfull
	                	if(fds[0].revents & POLLIN){
	                        	read(connection, buff, 4999); // reading from socket and printing (writing to stdout)
	                                write(1, buff, 5000);
	                                memset(buff,0,4999); // Clearing buffer after use
	                        }
	                        if(fds[1].revents & POLLIN){
	                        	read(0,buff,4999); // Reading from stdin and writing to socket
	                                write(connection,buff,5000);
	                                memset(buff, 0, 5000);  // Clearing buffer after use
	                        }
	                }
		}
// Technically won't reach but wanted just in case;
               	close(connection);
                return 0;
	}


	// If given an  two arguments which are assumed to be valid IP and port the running "client" code
	else if( argc == 3){
		char* ipAddress;
        	int port;
                ipAddress = argv[1];
                port = atoi(argv[2]);
//	Creating socket
                struct sockaddr_in ipaddr;
                ipaddr.sin_family = AF_INET;
                ipaddr.sin_port = htons(port);
                ipaddr.sin_addr.s_addr = inet_addr(ipAddress);
                int s  = socket(AF_INET, SOCK_STREAM, 0);
	// Connecting to socket of server
                connect(s ,  &ipaddr, sizeof(ipaddr));
                char* buff[5000];
        // Set up for poll
		struct pollfd fds[2];
        	int timeout_msecs = 60000;
		fds[0].fd = s;
              	fds[1].fd = 0;
                fds[0].events = POLLIN;
                fds[1].events = POLLIN;
		int ret;
                int i;
//		Handling reading and writing to socket
		while(1){
			ret = poll(fds, 2, timeout_msecs);
			if( ret >= 1){    //  if poll was succesfull

				if(fds[0].revents & POLLIN){
					read(s, buff, 4999); // reading from socket and printing (writing to stdout)
					write(1, buff, 5000);
					memset(buff,0,4999); // Clearing buffer after use
				}
				if(fds[1].revents & POLLIN){
                                        read(0,buff,4999); // Reading from stdin and writing to socket
					write(s,buff,5000);
					memset(buff, 0, 5000); // Clearing buffer after use
                                }
			}
		}
// Technically won't reach but wanted just in case;
		close(s);
		return 0;
	}
}

