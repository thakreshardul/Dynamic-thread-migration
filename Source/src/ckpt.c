#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ucontext.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/types.h>
//#include <dirent.h>
#include <mThread.h>
#include <stdlib.h>
#include <time.h>

//For socket communication
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "parser.h"

#define PORT 8555
#define HOST "127.0.0.1"
#define MAX_SERVER 10

#define SERVER_SELECT_HOST_1 HOST
#define SERVER_SELECT_HOST_2 HOST
#define SERVER_SELECT_HOST_3 HOST
#define SERVER_SELECT_HOST_4 HOST
#define SERVER_SELECT_HOST_5 HOST
#define SELECT_PORT_1 7777
#define SELECT_PORT_2 7778
#define SELECT_PORT_3 7779
#define SELECT_PORT_4 7780
#define SELECT_PORT_5 7781

static server** server_list;

char* ckpt_path="./bin/myckpt";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


server* select_server(int n)
{
  char* serv_1  = SERVER_SELECT_HOST_1;
  int port_1 = SELECT_PORT_1;
  server* server_1 = malloc(sizeof(server));
  server_1->IP = malloc(strlen(serv_1)+1);
  strcpy(server_1->IP, serv_1);
  server_1->port = port_1;

  char* serv_2  = SERVER_SELECT_HOST_2;
  int port_2 = SELECT_PORT_2;
  server* server_2 = malloc(sizeof(server));
  server_2->IP = malloc(strlen(serv_2)+1);
  strcpy(server_2->IP, serv_2);
  server_2->port = port_2;

  char* serv_3  = SERVER_SELECT_HOST_3;
  int port_3 = SELECT_PORT_3;
  server* server_3 = malloc(sizeof(server));
  server_3->IP = malloc(strlen(serv_3)+1);
  strcpy(server_3->IP, serv_3);
  server_3->port = port_3;

  char* serv_4  = SERVER_SELECT_HOST_4;
  int port_4 = SELECT_PORT_4;
  server* server_4 = malloc(sizeof(server));
  server_4->IP = malloc(strlen(serv_4)+1);
  strcpy(server_4->IP, serv_4);
  server_4->port = port_4;

  char* serv_5  = SERVER_SELECT_HOST_5;
  int port_5 = SELECT_PORT_5;
  server* server_5 = malloc(sizeof(server));
  server_5->IP = malloc(strlen(serv_5)+1);
  strcpy(server_5->IP, serv_5);
  server_5->port = port_5;

  int server_count = 5;

  server_list = malloc(server_count * sizeof(server));
  server_list[0] = server_1;
  server_list[1] = server_2;
  server_list[2] = server_3;
  server_list[3] = server_4;
  server_list[4] = server_5;

  // choose server using mod %
  int server_id = n % server_count;
  server* response_server = server_list[server_id];
//  printf("%d\n", server_id);

  return response_server;
}


void send_ckpt_to_server(){
	//srand(time(NULL));
  	//int randomnumber = rand() % MAX_SERVER;
	printf("requesting server for pid %d\n",getpid());
        server* serv = select_server(getpid());
        printf("sending to server %s:%d\n", serv->IP,serv->port);

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
	serv_addr.sin_addr.s_addr = inet_addr(serv->IP); ////M2 ka IP
	serv_addr.sin_port = htons(serv->port);

        
        FILE *fp = fopen("./bin/myckpt","rb");
        if(fp==NULL)
        {
            printf("File open error");
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
