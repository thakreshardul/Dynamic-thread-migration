#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <signal.h>

int count = 0;

void dostuff(int); 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int cpid=0;
int ppid=0;

int signal_child_handler(int sig){
    if(cpid!=0){
      //printf("in server handler cpid is %d\n", cpid);
      kill(cpid,SIGTSTP);
      cpid=0;
    }
}


int main(int argc, char *argv[])
{
  //printf("server pid is %d\n", getpid());
	int sockfd = 0;
    int connfd = 0;
    int bytesReceived = 0;
//    memset(recvBuff, '0', sizeof(recvBuff));
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
        return;
    }
	//printf("Listening...\n");
    printf("Server started on port %d\n", atoi(argv[1]));
  signal(SIGUSR1,signal_child_handler);
    while(1){
        printf("Listening for new connection...\n");
		connfd = accept(sockfd, (struct sockaddr*)NULL ,NULL);

		//printf("connection accepted\n");

	 /* Attempt a connection */
      int pid =fork();
		  if(pid == 0){
        //printf("server forked child pid is %d\n", getpid());
        cpid = getpid();
        dostuff(connfd);
        close(connfd);
        exit(0);
      }else{
        close(connfd);
      }
	}
	return 0;
}

void dostuff (int sockfd)
{
	int bytesReceived;
	//printf("In dostuff\n");

	char recvBuffer[4096];
	int fd = open("myckpt0", O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
	
	//printf("Opening myckpt0 file\n");
	while((bytesReceived = read(sockfd, recvBuffer, 4096)) > 0)
    {
        //printf("Bytes received %d\n",bytesReceived);    
        count++;
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
    static char *arg[]={"restart","myckpt0",NULL};
    //printf("execing\n");
    printf("Checkpoint image restarted\n");
    execv("./restart", arg);
}
