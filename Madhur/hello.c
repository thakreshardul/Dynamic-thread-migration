#define _GNU_SOURCE
#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include <linux/sched.h>
#include <sched.h>
#include <string.h>
//#include <bits/sched.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define MAX_STACK 1048576
#define HOST "10.110.16.65"

int cpid=0;
int ppid=0;

int signal_child_handler(int sig){
    if(cpid!=0){
      //printf("in handler cpid is %d\n", cpid);
      kill(cpid,SIGTSTP);
      cpid=0;
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


void send_result(void *buf){
	int clientSocket;
  	//char buffer[1024];
  	struct sockaddr_in serverAddr;
  	socklen_t addr_size;

  	/*---- Create the socket. The three arguments are: ----*/
  	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  	/*---- Configure settings of the server address struct ----*/
  	/* Address family = Internet */
  	serverAddr.sin_family = AF_INET;
  	/* Set port number, using htons function to use proper byte order */
  	serverAddr.sin_port = htons(8554);
  	/* Set IP address to localhost */
  	serverAddr.sin_addr.s_addr = inet_addr("10.110.16.65");//M1 machine IP
  	/* Set all bits of the padding field to 0 */
  	//memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  	/*---- Connect the socket to the server using the address struct ----*/
  	addr_size = sizeof serverAddr;
  	//printf("Connecting.....\n");
  	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  	/*---- Read the message from the server into the buffer ----*/
	  //printf("in send_result\n");
  	send(clientSocket, buf, strlen(buf), 0);

	//printf("sent\n");
	close(clientSocket);
	//printf("closing\n");
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
        size_t rd;
  	off_t size;
  	buffer = (char *)malloc(1024);
	int n = read(newsockfd, buffer, 1024);
	if (n < 0)
    		error("ERROR reading from socket");
	listening--;
 	}
  	return buffer;
}


void dostuff(int x, int term){
	//printf("Forked %d %d\n",x,term);
	int counter = 1;
        
	while(1){
  	//printf("%d\t", counter);
    if(counter == 2){
			//printf("checkpointing process %d\n", getpid());
			kill(getpid(), SIGUSR2);
			//printf("after checkpointing process %d\n", getpid());
      int status;
   			
 			status = remove("myckpt0");
 
 			if( status != 0 ){
				perror("Error");
			}
		}
    sleep(1);
    counter++;
    fflush(stdout);
    if (counter > 4) break;
  }
  double powx = 1;
	double facti = 1;
  int j;
  for(j = 1; j <= term ;j++){
		powx = powx * x;
		facti = facti * j;
	}
	double ans = powx/facti;
  printf("Sending back %d term: %lf", term, ans);
	//double ans = 27.963;        
	char resc[100];
  sprintf(resc, "%lf", ans);
  //printf("%d %d %lf sending %s\n", x,term,ans, resc);

	send_result(resc);
	//printf("back in do stuff after sending result\n");
  //exit(0);
  //kill(getpid(), SIGKILL);
  //raise(SIGKILL);
}

int main(int argc, char const *argv[])
{
  ppid = getpid();
  //printf("ppid is %d\n", ppid);
	int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     char *buffer;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 8554;//atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     	error("ERROR on binding listener");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);



	int x = atoi(argv[1]);
	int n = atoi(argv[2]);
  printf("Calculating result of e^%d for upto %d terms\n",x,n);
	int iterator;
	double answer = 0;
  signal(SIGUSR1,signal_child_handler);
	for(iterator=0;iterator<=n;iterator++){
    
     int status;
        
      status = remove("myckpt");
 
      if( status != 0 ){
        printf("Unable to delete the file\n");
        perror("Error");
      }

    pid_t pid = fork();
        
		if (pid == 0){
      int selfpid = getpid();
			//printf("In Child:%d\n", selfpid);
			dostuff(x,iterator);
			//printf("back in main\n");
      cpid=selfpid;
      //printf("child pid stored is %d and current pid is %d and parent pid is %d\n", cpid,getpid(),getppid());
      //printf("sending SIGUSR1 to parent\n");
      /*int status;
        
      status = remove("myckpt");
 
      if( status != 0 ){
        printf("Unable to delete the file\n");
        perror("Error");
      }*/
        
      kill(ppid,SIGUSR1);
      kill(getpid(),SIGTSTP);
      //sleep(2);
		}
		else{
    			int counter = 0;
          char *ans = (char *)malloc(1024);
      		ans = listen_for_result(sockfd, &cli_addr, &clilen);
      		double dubAns;
   	    	sscanf(ans, "%lf" , &dubAns);
      		//printf("received one term: %lf\n", dubAns);
          printf("Term %d of e^%d: %lf\n", iterator, x, dubAns);
			    answer += dubAns;
  		}
		//printf("sleeping for 1 second\n");
		sleep(1);
	}
	printf("Final answer is: %lf\n",answer);
	//printf("Now exiting from hello\n");
  return 0;
}
