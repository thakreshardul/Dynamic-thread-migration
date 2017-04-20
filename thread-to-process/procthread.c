#include "prcthd.h"

int procthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
	pid_t pid = fork();
	if(pid==0){
		printf("get pid in child is %d\n", getpid());
		printf("In child process\n");
		return pthread_create(thread,attr,start_routine,arg);
	}else{
		printf("%d\n", getpid());
		return -1;
	}
}

int procthread_join(pthread_t thread, void **retval){
	return pthread_join(thread,retval);
}

