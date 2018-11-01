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

int clilen = sizeof (struct sockaddr_in);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[256];

void *serve (void * newsockfd);
int main (int argc, char *argv[]) {
    // protected field
    int sockfd, portno;

	struct sockaddr_in serv_addr;
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf ("ERROR opening socket\n");
		exit (1); 
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	portno = 2680;
	serv_addr.sin_port = htons (portno);		
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf ("ERROR on binding\n");
		exit (1);
	}
	listen (sockfd, 5);
	printf ("INFO Server initiating...\n");
    // end protected field

    pthread_t thread1, thread2;
	pthread_create (&thread1, NULL, serve, (void*) &sockfd);
	pthread_create (&thread2, NULL, serve, (void*) &sockfd);
	printf ("INFO Server initiated.\n");
	while (1)
        wait (1);
	return 0;
}


void *serve (void *sockinp)
{
	struct sockaddr_in cli_addr;
	int sockfd = (int) (*((int*) sockinp));
    int newsockfd;

	FILE* sock_file;

	while (1)
	{
		newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);
		printf ("INFO Client connected.\n");
		sock_file = fdopen (newsockfd, "rw");
		
		while (1) {
			if (fgets (buffer, 256, sock_file) == NULL) {
				printf ("INFO Client disconnected.\n");
				fclose (sock_file);
				break;
			}
            pthread_mutex_lock (&mutex);
			printf ("INFO Client asked for the file ");
			fputs (buffer, stdout);

			FILE *fptr;
			int buffer_len = 0;
			char c;

			for (int i=0;i<256;i++)
				if (buffer[i]=='\n') {
					buffer[i] = 0;
					break;
				}
			fptr = fopen(buffer, "r"); 
			if (fptr == NULL) { 
				printf("ERROR Cannot open file, file name \"%s\"\n", buffer); 
				strcpy(buffer, "FAIL\n");
			} 
			else {
				bzero (buffer, 256);
				c = fgetc(fptr); 
				while (c != EOF) { 
					if(buffer_len == 256)
						break;
					buffer[buffer_len] = c;
					buffer_len++;
					c = fgetc(fptr); 
				} 
				fclose(fptr); 
			}

			printf ("INFO Sending response...\n");
			dprintf (newsockfd, "%s", buffer);
            pthread_mutex_unlock (&mutex);
		}
	}
}