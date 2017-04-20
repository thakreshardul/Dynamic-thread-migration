#define _GNU_SOURCE
#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include <linux/sched.h>
#include <sched.h>
//#include <bits/sched.h>
#include <signal.h>

#define MAX_STACK 1048576

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
                if (counter > 20) break;
        }
        return 1;
}

int main(int argc, char const *argv[])
{
	//pid_t pid = fork();
	//pthread_t thread1;
	//if (pid == 0){
	//	printf("In Child:%d\n", getpid());
		//dostuff();
		//pthread_create(&thread1, NULL, dostuff, NULL);
		//sleep(50);
		//pthread_join(thread1, NULL);	
	//}
	//else{
		char *stack = malloc(MAX_STACK);
		char *stackTop = stack + MAX_STACK;		
		clone(&dostuff, stackTop, CLONE_FS, 0);
		sleep(30);
		free(stack);
		printf("In parent:%d\n", getpid());
		exit(0);
	//}
	printf("Now exiting from hello\n");
	return 0;
}
