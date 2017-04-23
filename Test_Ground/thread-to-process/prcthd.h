#ifndef PRCTHD_H
#define PRCTHD_H

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int procthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
int procthread_join(pthread_t thread, void **retval);

#endif
