/*
FILE: 
    p2p.c
DESCRIPTION: 
    The implementation of p2p model. 
    Creates one server thread and one client thread. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "global.h"
#include "client.h"
#include "server.h"

int main (int argc, char *argv[]) {
    // protected field
    int sockfd, portno;

    char portbuffer[15];
	printf( BLUE "[SERVER] " NONE "Please assign a port number: \n");
	fgets(portbuffer,sizeof(portbuffer),stdin);
    portno = atoi(portbuffer);

	struct sockaddr_in serv_addr;
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf ( BLUE "[SERVER] ERROR opening socket\n");
		exit (1); 
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (portno);		
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf ( BLUE "[SERVER] ERROR on binding\n");
		exit (1);
	}
	listen (sockfd, 5);
	printf ( BLUE "[SERVER] INFO Server initiating...\n");
    // end protected field

    pthread_t thread1, thread2;
	pthread_create (&thread1, NULL, serve, (void*) &sockfd);
    printf ( BLUE "[SERVER] INFO Server initiated.\n");
	pthread_create (&thread2, NULL, client, NULL);
	printf ( CYAN "[CLIENT] INFO Client initiated.\n");
	while (1)
        wait (1);
	return 0;
}

