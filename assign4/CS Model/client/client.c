/*
FILE: 
    client.c
DESCRIPTION: 
    The implementation of client. 
    Connects to the host, copy from standard input to buffer, and then copy from socket to buffer.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int write_file(char *path, const char *text) {
	for (int i=0; i<sizeof(path)/sizeof(char); i++) {
		if (path[i] == '\n') {
			path[i] = '\0';
			break;
		}
	}

	FILE *f = fopen(path, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
				exit(1);
	}

	fprintf(f, "%s", text);
	fclose(f);
}

int main (int argc, char *argv[])
{
	// start connecting
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	char path[256];

	portno = 2680;
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // specifies the protocol to be TCP
	if (sockfd < 0) {
		printf ("ERROR opening socket.");
		exit (1);
	}

	char hostaddr[15];  // "127.0.0.1";
	printf("Please input your host address: ");
	fgets(hostaddr,sizeof(hostaddr),stdin);

	server = gethostbyname(hostaddr);
	if (server == NULL) {
		printf ("ERROR, no such host.\n");
		exit (0);
	}

	bzero ((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy ((char*) server -> h_addr, (char*) &serv_addr.sin_addr.s_addr,
		server -> h_length);
	serv_addr.sin_port = htons (portno);

	if (connect (sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf ("ERROR connecting");
		exit (1);
	}
	// end connecting

	while (1) {
		printf ("To get a file, insert its path: \n");
		fgets (path, 255, stdin);

		bzero (buffer, 256);
		strcpy (buffer, path);
		if (buffer[0] == 27) { // ESC
			break;
		}

		int n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) {
			printf("ERROR writing buffer\n");
			exit(1);
		}
		bzero(buffer, 256);

		n = read(sockfd, buffer, 255);
		if (n < 0) {
			printf ("ERROR reading buffer\n");
		} else if (buffer[0] == 'F') {
			printf("From server: FAILED\n");
		} else {
			write_file(path, buffer);  // todo
			printf("From server: file saved.\n");
		}
	}
	close (sockfd);
	return 0;
}
