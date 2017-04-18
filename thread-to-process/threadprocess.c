#include <pthread.h>
#include <stdio.h>

/* this function is run by the second thread */
void *inc_x(void *x_void_ptr)
{

	/* increment x to 100 */
	int *x_ptr = (int *)x_void_ptr;
	while(++(*x_ptr) < 100);

	printf("x increment finished\n");

	/* the function must return something - NULL will do */
	return NULL;

}

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

int main()
{

	int x = 0, y = 0;

	/* show the initial values of x and y */
	printf("x: %d, y: %d\n", x, y);

	/* this variable is our reference to the second thread */
	pthread_t inc_x_thread;

	/* create a second thread which executes inc_x(&x) */
	int ret = procthread_create(&inc_x_thread, NULL, inc_x, &x);
	if(ret>0) {

	fprintf(stderr, "Error creating thread\n");
	return 1;

	}
	/* increment y to 100 in the first thread */
	while(++y < 100);

	printf("y increment finished\n");

	/* wait for the second thread to finish */
	if(ret!=-1 && procthread_join(inc_x_thread, NULL)) {

	fprintf(stderr, "Error joining thread\n");
	return 2;

	}

	/* show the results - x is now 100 thanks to the second thread */
	printf("x: %d, y: %d\n", x, y);

	return 0;

}