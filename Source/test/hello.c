#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mThread.c>

typedef struct arguments
{
  int arg1;
  int arg2;
}arguments;

void calculate_binomial(arguments *ptr){
  printf("calculate_binomial: %d^%d\n", ptr->arg1, ptr->arg2);
  double powx = 1;
  double facti = 1;
  int j;
  for(j = 1; j <= ptr->arg2 ;j++){
    powx = powx * ptr->arg1;
    facti = facti * j;
  }
  double ans = powx/facti;
  printf("answer is %lf\n",ans);
  mthread_exit(&ans,sizeof(ans));
}

int main(int argc, char const *argv[])
{
  mthread_configure();
  int x = atoi(argv[1]);
  int n = atoi(argv[2]);
  mthread_t thread_id[n];
  double final = 0;
  void *answer = malloc(sizeof(double));
  arguments args;
  printf("Calculating result of e^%d for upto %d terms\n",x,n);
  for (int i = 0; i <= n; i++)
  {
    args.arg1 = x;
    args.arg2 = i;
    mthread_create(&thread_id[i], NULL, (void*)&calculate_binomial, &args);
//	sleep(2);
//  }
//  for (int i = 0; i <=n ; ++i)
//  {
    mthread_join(&thread_id[i], answer);
    final += *((double*)answer);
  }
  
  printf("Final answer is: %lf\n",final);
  return 0;
}
