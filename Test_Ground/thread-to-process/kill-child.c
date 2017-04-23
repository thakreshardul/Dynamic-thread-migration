#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sched.h>

pid_t parent_pid;

void sigquit_handler(int sig){
	int self_pid = getpid();
	if(self_pid!=parent_pid){
		printf("exiting child with pid %d\n", self_pid);
		int timer = 1;
		while(timer<30){
			printf("%d\n", timer);
			timer++;
			sleep(1);
		}
		_exit(0);
	}
	printf("in process with pid %d\n", self_pid);
}

int main(){
	int x=0;
	int y=0;
	parent_pid = getpid();
	signal(SIGQUIT,sigquit_handler);

	pid_t pid = fork();
	if(pid==0){		//child
		printf("in child with pid %d\n", getpid());
	}else if(pid==-1){
		printf("error while forking\n");
	}else{
		printf("in parent with pid %d\n", getpid());
	}
	sleep(1);
	printf("killing children\n");
	kill(-parent_pid,SIGQUIT);

	int status;
	pid_t child = wait(&status);
	if (child > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		printf("child %d successfully quit\n", child);
	}
	sleep(5);
	printf("DONE\n");
}