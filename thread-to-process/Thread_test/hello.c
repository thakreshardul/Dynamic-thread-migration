#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <errno.h>

pid_t global_pid;

void doSomeThing()
{
    int i;
    do{
        printf("I:%d\n", i);
        i++;
    }while(i < 20);
    return NULL;
}

int main(void)
{
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        global_pid = getpid();
        printf("Child PID:%d\n", global_pid);
        raise(SIGUSR2);
        doSomeThing();
        exit(0);
    }
    else
    {
        printf("Parent PID:%d\n", getpid());
        exit(0);
    }

    /*int i = 0;
    int err;
    printf("Parent PID: %d\n", getpid());
    printf("Parent TID: %ld\n", pthread_self());
    while(i < 2)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

        i++;
    }

    
    ucontext_t ucp;
    if(getcontext(&ucp) == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

    printf("Got context\n");
    
    //sleep(60);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
	
        printf("UCLINK: %x\n", ucp.uc_link);
    printf("\n%p\n", ucp.uc_link->uc_stack.ss_sp);
    ucp.uc_link = 0;
    ucp.uc_stack.ss_sp = malloc(MEM);
    ucp.uc_stack.ss_size = MEM;
    ucp.uc_stack.ss_flags = 0;
    makecontext(&ucp, (void *)&doSomeThing, 0);
    printf("Made context\n");
    int test = 1;
    if (test)
    {
        setcontext(&ucp);
        test--;
    }
    return 0;*/
}
