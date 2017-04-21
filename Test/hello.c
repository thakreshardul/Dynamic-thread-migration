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


void send_result(char *buffer){

	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server = mmap(NULL, sizeof(hostent), PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    printf("Pointer address: %p\n", server);
    portno = 8554;
    printf("creating socket\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    	error("ERROR opening socket");
    
    printf("Getting Hostname\n");
    int test;
    //server = gethostbyaddr("127.0.0.1", sizeof("127.0.0.1"), AF_INET);
    //server->h_name = "127.0.0.1";
    //server->h_addrtype = 2;
    //server->h_length = 4;
    //printf("%s\n", );
    //struct hostent *he;
	//struct in_addr ipv4addr;
	//struct in6_addr ipv6addr;

	//inet_pton(AF_INET, "127.0.0.1", &ipv4addr);
	//server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	//printf("Host name: %s\n", server->h_name);

	//inet_pton(AF_INET6, "2001:db8:63b3:1::beef", &ipv6addr);
	//he = gethostbyaddr(&ipv6addr, sizeof ipv6addr, AF_INET6);
	//printf("Host name: %s\n", he->h_name);
	printf("Stack top: %p\n", &test);
	int test1;
	printf("Stack top2: %p\n", &test1);

    server = gethostbyname(HOST);
    printf("Got Hostname\n");
    if (server == NULL) {
    	fprintf(stdout,"ERROR, no such host\n");
    	exit(0);
    }
    printf("connecting to server\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);

    serv_addr.sin_port = htons(portno);

    //if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    //    error("ERROR connecting");
    
    /*int len = strlen(buffer);
    //n = write(sockfd, &len, sizeof(len));
    n = write(sockfd, buffer, len);

	if (n < 0) 
        error("ERROR writing to socket");

    close(sockfd);
    free(buffer);*/
    printf("In send_result: %s\n", buffer);

}

/*
char *listen_for_result(){

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
       error("ERROR on binding");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     int listening = 1;
     while (listening) {
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  
     	if (newsockfd < 0) 
        	error("ERROR on accept");
    	printf("Server accepted connection....\n");
    	size_t rd;
  		off_t size;
  		//printf("reading\n");
  		//rd = read(newsockfd, &size, sizeof(off_t));
  		//printf("Length is: %d\n", size);
  		//printf("reading complete\n");
  		//if (rd < 0)
    	//	error("ERROR reading from socket");

		buffer = (char *)malloc(1024);

		//bzero(buffer,256);
  		printf("reading Result\n");
 		int n = read(newsockfd, buffer, 6);
 		//		close(sockfd);
  		if (n < 0)
    		error("ERROR reading from socket");
    	printf("Answerrrrrrrrrrrr-------------%s\n", buffer);
  		printf("Result read\n");
  		listening--;
  	}
  	return buffer;
}
*/

int dostuff(){
	printf("cloned\n");
	printf("Child PID: %d\n", getpid());
	int counter = 1;
	while(1)
        {
        	printf("%d\t", counter);
                sleep(1);
                counter++;
                fflush(stdout);
                if (counter > 10) break;
        }
        //return 1;
	send_result("Result");
	return 1;
}

int main(int argc, char const *argv[])
{
	pid_t pid = fork();
	//pthread_t thread1;
	if (pid == 0){
	//	printf("In Child:%d\n", getpid());
		dostuff();
		//pthread_create(&thread1, NULL, dostuff, NULL);
		//sleep(50);
		//pthread_join(thread1, NULL);
		goto ret;	
	}
	else{
		

		/*char *stack = malloc(MAX_STACK);
		printf("Stack end: %p\n", stack);
		char *stackTop = stack + MAX_STACK;		
		printf("Stack start: %p\n", stackTop);
		printf("Stack size: %ld\n", stackTop - stack);
		clone(&dostuff, stackTop, 0, 0);
		
		//char *answer = (char *)malloc(1024);
		//answer = listen_for_result();
		sleep(30);
		free(stack);*/
		printf("In parent:%d\n", getpid());
	}
		//printf("Answer is:%s\n", answer);
		//exit(0);
	//}
	printf("Now exiting from hello\n");
ret:	return 0;
}
