#define BLUE                 "\e[0;34m"
#define CYAN                 "\e[0;32m"
#define NONE                 "\e[0m"

int clilen = sizeof (struct sockaddr_in);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[256];