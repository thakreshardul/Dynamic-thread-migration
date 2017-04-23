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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


void send_ckpt_to_server(){
	int fd = open("myckpt", O_RDONLY);
	struct stat st;
	stat("myckpt", &st);
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
    /*printf("---------------------------------------------\n");
	printf("Official name is: %s\n", server->h_name);
    printf("    IP addresses: ");
    char **addr_list = server->h_addr_list;
    for(int i = 0; addr_list[i] != NULL; i++) {
        printf("%d ", (*addr_list[i]));
    }
    printf("---------------------------------------------\n");
	*/
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
	printf("file size sent to server %ld\n",file_size);

    n = write(sockfd, &file_size, sizeof(off_t));
    n = write(sockfd, buffer, file_size);
	printf("size sent to server %ld and size of buffer %ld\n",n,strlen(buffer));

	if (n < 0) 
        error("ERROR writing to socket");

    close(sockfd);
    free(buffer);
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
		checkpoint = open("myckpt", O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
		
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
