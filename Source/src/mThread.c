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
#include <string.h>
#include <stdlib.h>
#include <mThread.h>

#define MEM 64000

#define PORT 4747
#define HOST "127.0.0.1"

// global data, common for all processes
static int process_count = 0;


// process specific port for sending response
static server* response_server;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}


static int sockfd;
static struct sockaddr_in serv_addr;


void mthread_configure()
{ 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) 
	error("ERROR opening socket");

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding listener");

  listen(sockfd,5);
}


int mthread_create(mthread_t *mt, const mthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
  int status;
  mt = malloc(sizeof(mthread_t));

  if(process_count > 0){
    status = remove("./bin/myckpt");  
    if(status != 0)
      perror("Error deleting checkpoint file in process 1");
  }

  process_count++;

  pid_t pid = fork();
  if (pid == 0){
        int selfpid = getpid();
        sleep(1);
	
 	// raise signal for checkpoint
	raise(SIGUSR2);
	sleep(1);

	status = remove("./bin/myckpt0");
	if(status != 0)
	  perror("Error deleting checkpoint file in process 2");

        

	// call the funtions requested to be run in thread
	(*start_routine)(arg);

	kill(selfpid, SIGTSTP);
	kill(getpid(),SIGTSTP);
  }
  else{
        mt->pid = pid;
  }

  return 0;
}


int mthread_join(mthread_t* mt, void *retval)
{
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) {
     error("ERROR on accept");
  }
  void *buffer = malloc(1024);
  int n = read(newsockfd, buffer, 1024);
  if (n < 0)
    error("ERROR reading from socket");
  memcpy(retval, buffer,n);
  return 0;
}



void mthread_exit(void *retval, int size)
{
  printf("in mthread exit with retval %lf\n",*((double*)retval));
  sleep(1);
  // sends result to parent process
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  send(clientSocket, retval, size, 0);

  close(clientSocket);
}




