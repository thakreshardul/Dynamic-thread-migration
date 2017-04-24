

int mthread_create(mthread_t *mt, const mthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

int mthread_join(mthread_t thread, void *retval);

void mthread_exit(void *retval);
