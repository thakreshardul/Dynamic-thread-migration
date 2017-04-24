#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>


void dostuff(int); 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*
int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
       error("ERROR on binding");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     while (1) {
       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  
       if (newsockfd < 0) 
         error("ERROR on accept");
       printf("Server accepted connection....\n");
         pid = fork();
         if (pid < 0)
           error("ERROR on fork");
         if (pid == 0){
           close(sockfd);
           dostuff(newsockfd);
           exit(0);
         }
         else
           close(newsockfd);
     }
     close(sockfd);
     return 0; 
}
*/

int main(int argc, char *argv[])
{
	int sockfd = 0;
	int connfd = 0;
	int bytesReceived = 0;

	struct sockaddr_in serv_addr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}

	/* Initialize sockaddr_in data structure */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1])); // port
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

	//printf("Binding done\n");
	if(listen(sockfd, 10) == -1)
	{
		printf("Failed to listen\n");
		return 1;
	}
	//printf("Listening...\n");
	printf("Server started on port %d\n", atoi(argv[1]));
	//signal(SIGUSR1,signal_child_handler);
	while(1){
		printf("Listening for new connection...\n");
		connfd = accept(sockfd, (struct sockaddr*)NULL ,NULL);

		//printf("connection accepted\n");

		/* Attempt a connection */
		int pid =fork();
		if(pid == 0){
			//printf("server forked child pid is %d\n", getpid());
			//cpid = getpid();
			dostuff(connfd);
			close(connfd);
			exit(0);
		}else{
			close(connfd);
		}
	}
	return 0;
}

/*
void dostuff (int sock)
{
  size_t n;
  off_t size;
  printf("reading\n");
  n = read(sock, &size, sizeof(off_t));
  printf("reading complete\n");
  if (n < 0)
    error("ERROR reading from socket");

  char *buffer = (char *)malloc(size);


  //bzero(buffer,256);
  printf("reading ckpt\n");
  n = read(sock,buffer,size);
  printf("ckpt read\n");
  if (n < 0)
    error("ERROR reading from socket");

  //int fd = open("./bin/myckpt", O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
  //write(fd, buffer, n);
  //close(fd);
  static char *arg[]={"restart","./bin/myckpt",NULL};
  printf("execing\n");
  execv("./bin/restart", arg);
}

*/

void dostuff (int sockfd)
{
	int bytesReceived;
	//printf("In dostuff\n");

	char recvBuffer[4096];
	int fd = open("./bin/myckpt0", O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
	
	//printf("Opening myckpt0 file\n");
	while((bytesReceived = read(sockfd, recvBuffer, 4096)) > 0)
    {
		write(fd,recvBuffer,bytesReceived);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
	close(fd);
    //printf("count: %d\n",count);
	//printf("on server forked child before exec pid is %d \n", getpid());
	//return;
    static char *arg[]={"restart","./bin/myckpt0",NULL};
    //printf("execing\n");
    printf("Checkpoint image restarted\n");
    execv("./bin/restart", arg);
}

