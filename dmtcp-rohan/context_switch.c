
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <pthread.h>
#include <unistd.h>

void* doSomething1()
{
  //while(1)
  //{ 
    printf("in process: %d in thread1: %d\n", (int)getpid(), (int)pthread_self());
    //ucontext_t context;
    //getcontext(&context);
    //setcontext(context.uc_link);
    printf("after setting context");
    sleep(2);
	printf("After sleep");
  //}
}

void* doSomething2()
{
  //while(1)
  //{ 
    printf("in process: %d in thread2: %d\n", (int)getpid(), (int)pthread_self());
    sleep(2);
 // }
}


int main()
{
  ucontext_t *ucp = (ucontext_t *)malloc(sizeof(ucontext_t));
 // int s = getcontext(ucp);

  pthread_t p1, p2;
  int res1 = pthread_create (&p1, NULL, doSomething1, NULL);
    getcontext(ucp);
  pthread_join(p1, NULL);
  
int res2 = pthread_create (&p2, NULL, doSomething2, NULL);

  //while(1)
  //{
    printf("in main: %d in thread: %d\n", (int)getpid(), (int)pthread_self());
   // sleep(2);
 // }
//    getcontext(ucp);

  pthread_join(p2, NULL);

  //if(ucp->uc_link)
  printf("uclink:%ld\n", (long)ucp->uc_link);
  setcontext(ucp);

  
  return 0; 
}
