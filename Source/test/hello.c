#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mThread.h>

typedef struct arguments
{
  int arg1;
  int arg2;
}arguments;

void calculate_binomial(arguments *ptr){
  double powx = 1;
  double facti = 1;
  int j;
  for(j = 1; j <= ptr->arg2 ;j++){
    powx = powx * ptr->arg1;
    facti = facti * j;
  }
  double ans = powx/facti;
  mthread_exit(&ans);
}

int main(int argc, char const *argv[])
{
  mthread_configure();
  int x = atoi(argv[1]);
  int n = atoi(argv[2]);
  mthread_t thread_id[n];
  double answer, final = 0;
  arguments args;
  printf("Calculating result of e^%d for upto %d terms\n",x,n);
  for (int i = 0; i <= n; i++)
  {
    args.arg1 = x;
    args.arg2 = i;
    mthread_create(&thread_id[i], NULL, (void*)&calculate_binomial, &args);
  }
  for (int i = 0; i <=n ; ++i)
  {
    mthread_join(&thread_id[i], &answer);
    final += answer;
  }
  
  printf("Final answer is: %lf\n",answer);
  return 0;
}
