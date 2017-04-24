
struct server{
  char* IP;
  int port;
};
typedef struct server server;

struct mthread_t{
  pid_t pid;
  int sockID;
  server sv;
};
typedef struct mthread_t mthread_t;

typedef	void * (*mthread_startroutine_t)(void *);
typedef void * mthread_attr_t;


void mthread_configure();

int mthread_create(mthread_t *mt, const mthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);

int mthread_join(mthread_t* thread, void *retval);

void mthread_exit(void *retval);
