#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>

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
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

void dostuff (int sock)
{
   int n;
   char buffer[256];
   char input[256];
   char *p;
   int x = 0;
   float msec = 0;
   struct timeval start, end;
   gettimeofday(&start, NULL);
   gettimeofday(&end, NULL);

   msec = (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec-start.tv_usec)/(float)1000;
   printf("time %0.2f \n", (float)end.tv_sec);
   
   char *family1[2] = {"aaa", "bbb"};

   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   for (x =0; x<256; x++){   
	input[x] = buffer[x];
   }
   if ((p = strchr(input, '\n')) != NULL) {
        *p = '\0';
    }

   char args[3][15];
   int a = 0;
   int i = 0;
   int index = 0;
   while (input[a] != '\0'){
	   if (input[a] == ' '){
		   args[index][i] = '\0';
		   i=0;
		   a++;
		   index++;
	   }
	   
	   args[index][i++] = input[a++];
   }
   
   printf("Message from client: %s\n",input);
   
   n = write(sock,"Received",18);
   if (n < 0) error("ERROR writing to socket");
}
