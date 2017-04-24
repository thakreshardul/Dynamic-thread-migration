

int mthread_create(mthread_t *mt, const mthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
