
typedef struct semaphore
{
    int val;
    struct process** queue;
}semaphore_t,*psemaphore_t;