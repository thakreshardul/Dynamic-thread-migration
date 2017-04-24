#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <dlfcn.h>

//For socket communication
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "parser.h"

#define PORT 8555
#define HOST "127.0.0.1"

char* ckpt_path="./bin/myckpt";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*
void send_ckpt_to_server(){
	int fd = open(ckpt_path, O_RDONLY);
	struct stat st;
	stat(ckpt_path, &st);
	off_t file_size = st.st_size;
	char *buffer = (char *)malloc(file_size);

	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = PORT;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    	error("ERROR opening socket");
    
    server = gethostbyname(HOST);
 
    if (server == NULL) {
    	fprintf(stderr,"ERROR, no such host\n");
    	exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    bzero(buffer, file_size);
    read(fd, buffer, file_size);

    n = write(sockfd, (off_t *)file_size, sizeof(file_size));
    n = write(sockfd, buffer, file_size);

	if (n < 0) 
        error("ERROR writing to socket");

    close(sockfd);
    free(buffer);
}
*/

void send_ckpt_to_server(){
	//printf("In send_ckpt_to_server\n");
	int listenfd = 0;
	int connfd = 0;
	struct sockaddr_in serv_addr;
	char sendBuffer[4096];
	int numrv;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	//printf("Socket retrieve success\n");

	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuffer, '0', sizeof(sendBuffer));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(HOST); ////M2 ka IP
	serv_addr.sin_port = htons(PORT);

	
    //while(1)
    //{
        
        FILE *fp = fopen("myckpt","rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return;   
        }   

	  if(connect(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){

		printf("\n Error : Connect Failed \n");
        return;
       }else{
	printf("Sending checkpoint image to server\n");
       while(1)
        {
            int nread = fread(sendBuffer,1,4096,fp);
            //printf("Bytes read %d \n", nread);        

            if(nread > 0)
            {
                //printf("Sending \n");
                
                write(listenfd, sendBuffer, nread);
            }

            if (nread < 4096)
            {
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }


        }
		//printf("All bytes sent. closing connection\n");
        //close(listenfd);
        sleep(1);

	}

}


void signal_handler (int signal)
{
	printf("-----------------IN SIGNAL HANDLER-----------------\n");
	if (signal == SIGUSR2)
	{
		int process, checkpoint;
		MemoryRegion *proc = (MemoryRegion *)malloc(sizeof(MemoryRegion));
		ucontext_t context, *cp=&context;
		process = open("/proc/self/maps", O_RDONLY);
		if (process == -1)
			fprintf(stderr, "Error openeing /proc/self/maps:%s\n", strerror(errno));
		checkpoint = open(ckpt_path, O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
		
		if (checkpoint == -1)
			fprintf(stderr,"Error opening checkpoint%s\n", strerror(errno));
		while(1)
		{
			if (readline(process, proc) == 0)
				break;
				
			if ((proc->isReadable == 1) && (!(strstr(proc->name, "vsys"))))
			{	
				if (write(checkpoint, proc, sizeof(MemoryRegion)) == -1) 
				{
					fprintf(stderr, "Unable to write struct%s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				ssize_t wrote = write(checkpoint, proc->startAddr, proc->size * sizeof (char));
				if (wrote == -1) 
				{
					fprintf(stderr, "Unable to write data%s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
			
				
			}
  		}
  		free(proc);	
  		int pid = getpid();
  		if(getcontext(cp) == -1)
		{
			fprintf(stderr, "%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (getpid() != pid)
		{
			return;
		}
		
		ssize_t bytes = write(checkpoint, &context, sizeof(ucontext_t));
		if (!bytes)
		{
			fprintf(stderr, "Context not written%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		close(checkpoint);
		send_ckpt_to_server();
	}
	printf("\n");
	exit(0);
	
}

__attribute__ ((constructor))
void checkpoint_constructor(){
	signal(SIGUSR2, signal_handler);
}
