#include <signal.h>
#include "prcthd.h"
#define MAX_THREAD 2

int n = 0;


int procthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
	pid_t pid = fork();
	if(pid==0){
		printf("get pid in child is %d\n", getpid());
		printf("In child process\n");
		n++;
		printf("sending signal SIGUSR2 to \n");
		kill(getpid(), SIGUSR2);
		int ret = pthread_create(thread, attr, start_routine, arg);

		return ret;
	}else{
		printf("get pid in parent is %d\n", getpid());
		return -1;
	}
}

int procthread_join(pthread_t thread, void **retval){
	return pthread_join(thread,retval);
}

