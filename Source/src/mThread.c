#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sched.h>
#include <sched.h>
#include <strings.h>
#include <avcall.h>

// keep track of multiple servers

#define MEM 64000


struct mthread_t{
  pid_t pid;
};

typedef struct mthread_t mthread_t;

typedef	void * (*pthread_startroutine_t)(void *);
typedef void * pthread_addr_t;


char *listen_for_result(int sockfd, struct sockaddr_in * cli_addr, socklen_t * clilen);


void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int mthread_create(mthread_t *mt, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
  pid_t pid = fork();
  if (pid == 0){

	(*start_routine)(arg);
	raise(SIGUSR2);
  }
  else{
	int sockfd, portno;
     	struct sockaddr_in serv_addr;

     	sockfd = socket(AF_INET, SOCK_STREAM, 0);

     	if (sockfd < 0) 
        	error("ERROR opening socket");

     	bzero((char *) &serv_addr, sizeof(serv_addr));
     	portno = 8554;
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(portno);

     	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     		error("ERROR on binding listener");

     	listen(sockfd,5);
  }
  
  
  mt = malloc(sizeof(mthread_t));
  mt->pid = pid;

  return 0;
}


char *listen_for_result(int sockfd, struct sockaddr_in * cli_addr, socklen_t * clilen){

	
     int newsockfd;
     int listening = 1;
     char *buffer;
     while (listening) {
     	newsockfd = accept(sockfd, (struct sockaddr *) cli_addr, clilen);
     	if (newsockfd < 0) {
		error("ERROR on accept");
	}
  	buffer = (char *)malloc(1024);
	int n = read(newsockfd, buffer, 1024);
	if (n < 0)
    		error("ERROR reading from socket");
	listening--;
 	}
  	return buffer;
}

