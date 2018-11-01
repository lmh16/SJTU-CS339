#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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
		printf( CYAN "[SERVER] Error opening file!\n");
		exit(1);
	}

	fprintf(f, "%s", text);
	fclose(f);
    return 0;
}

void *client ()
{
    // start connecting
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	char path[256];

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // specifies the protocol to be TCP
	if (sockfd < 0) {
		printf ( CYAN "[CLIENT] ERROR opening socket.");
		exit (1);
	}

	char hostaddr[15];  // "127.0.0.1";
    char portbuffer[15];
	printf( CYAN "[CLIENT] " NONE "Please input your host address: \n");
	fgets(hostaddr,sizeof(hostaddr),stdin);
	printf( CYAN "[SERVER] " NONE "Please input its port number: \n");
	fgets(portbuffer,sizeof(portbuffer),stdin);
    portno = atoi(portbuffer);

	server = gethostbyname(hostaddr);
	if (server == NULL) {
		printf ( CYAN "[CLIENT] ERROR, no such host.\n");
		exit (0);
	}

	bzero ((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy ((char*) server -> h_addr, (char*) &serv_addr.sin_addr.s_addr,
		server -> h_length);
	serv_addr.sin_port = htons (portno);

	if (connect (sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf ( CYAN "[CLIENT] ERROR connecting");
		exit (1);
	}
	// end connecting

	while (1) {
		printf ( CYAN "[CLIENT] " NONE "To get a file, insert its path: \n");
		fgets (path, 255, stdin);

		bzero (buffer, 256);
		strcpy (buffer, path);
		if (buffer[0] == 27) { // ESC
			break;
		}

		int n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) {
			printf( CYAN "[CLIENT] ERROR writing buffer\n");
			exit(1);
		}
		bzero(buffer, 256);

		n = read(sockfd, buffer, 255);
		if (n < 0) {
			printf ( CYAN "[CLIENT] ERROR reading buffer\n");
		} else if (buffer[0] == 'F') {
			printf( CYAN "[CLIENT] INFO Server returns FAILED\n");
		} else {
			write_file(path, buffer);  // todo
			printf( CYAN "[CLIENT] INFO File saved from server.\n");
		}
	}
	close (sockfd);
	return 0;
}