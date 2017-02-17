/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define BUFFER_SIZE BUFSIZ /* or some other number */



struct struct_for_recv{
	int skfd;
	char buffer[MAXDATASIZE];
//	int buffer;
};

void *recv_print(struct struct_for_recv *s){
	int numbytes;
//	int sockfd=s->skfd;
//	char buf[MAXDATASIZE]=s.buffer;
//	char *buf=s->buffer;

    while(1){

	if (recv(s->skfd, s->buffer, MAXDATASIZE-1, 0) == -1) {
	    perror("recv");
	    exit(1);
	}
	else // !=-1
	    printf("%s\n",s->buffer);
	
    }//end while	    
	
}



void *type_get(int sfd){
    while(1){
	char message[]="";
	fgets(message, 40, stdin);
	send(sfd, message, 40, 0);
    }//end while
}




// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure


    	pthread_t tid, tid2;

	struct struct_for_recv mys;

	mys.skfd=sockfd;


        pthread_create(&tid, NULL, recv_print, &mys);
//	pthread_create(&tid2, NULL, type_get, sockfd);


    	pthread_exit(NULL);
	pthread_create(&tid2, NULL, type_get, sockfd);


	close(sockfd);

	return 0;
}
