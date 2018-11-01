#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void *serve (void *sockinp)
{
	struct sockaddr_in cli_addr;
	int sockfd = (int) (*((int*) sockinp));
    int newsockfd;

	FILE* sock_file;

	while (1)
	{
		newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);
		printf ( BLUE "[SERVER] INFO Client connected.\n");
		sock_file = fdopen (newsockfd, "rw");
		
		while (1) {
			if (fgets (buffer, 256, sock_file) == NULL) {
				printf ( BLUE "[SERVER] INFO Client disconnected.\n");
				fclose (sock_file);
				break;
			}
            pthread_mutex_lock (&mutex);
			printf ( BLUE "[SERVER] INFO Client asked for the file ");
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
				printf( BLUE "[SERVER] ERROR Cannot open file, file name \"%s\"\n", buffer); 
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

			printf ( BLUE "[SERVER] INFO Sending response...\n");
			dprintf (newsockfd, "%s", buffer);
            pthread_mutex_unlock (&mutex);
		}
	}
}
