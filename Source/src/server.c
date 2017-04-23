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
     }/* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

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
