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
#define HOST "127.0.0.1"


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
  	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  	/* Set all bits of the padding field to 0 */
  	//memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  	/*---- Connect the socket to the server using the address struct ----*/
  	addr_size = sizeof serverAddr;
  	printf("Connecting.....\n");
  	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  	/*---- Read the message from the server into the buffer ----*/
	printf("in send_result\n");
  	send(clientSocket, buf, strlen(buf), 0);
	close(clientSocket);
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


void dostuff(int x, int i){
	printf("Forked\n");
	int counter = 1;
        
	while(1)
        {
        	printf("%d\t", counter);
                if(counter == 5){
			printf("killing process %d\n", getpid());
			kill(getpid(), SIGUSR2);
			printf("after killing process %d\n", getpid());
                        int status;
   			
   			status = remove("myckpt");
 
   			if( status == 0 )
      				printf("myckpt file deleted successfully.\n");
			   else
			   {
			      printf("Unable to delete the file\n");
			      perror("Error");
			   }
			}
                sleep(1);
                counter++;
                fflush(stdout);
                if (counter > 7) break;
        }
        double powx = 1;
	double facti = 1;
        int j;
        for(j = 1; j <= i ;j++){
		powx = powx * x;
		facti = facti * j;
	}
	double ans = powx/facti;
        char resc[100];
        sprintf(resc, "%lf", ans);
	send_result(resc);
  exit(0);
}

int main(int argc, char const *argv[])
{

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
	int iterator;
	double answer = 0;
	for(iterator=0;iterator<=n;iterator++){
		pid_t pid = fork();
        
		if (pid == 0){
			printf("In Child:%d\n", getpid());
			dostuff(x,iterator);
		}
		else{
    			int counter = 0;
      			char *ans = (char *)malloc(1024);
      			ans = listen_for_result(sockfd, &cli_addr, &clilen);
      			double dubAns;
   	    	         sscanf(ans, "%lf" , &dubAns);
      			printf("received one term: %lf\n", dubAns);
			answer += dubAns;
  		}
		printf("sleeping for 1 second\n");
		sleep(1);
	}
	printf("Final answer is %lf\n",answer);
	printf("Now exiting from hello\n");
  return 0;
}
