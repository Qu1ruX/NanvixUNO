#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Executes atomic operations on a semaphore
 */
PUBLIC int semop(int semid, int op)
{
    pSemaphore_t sema=getSemWithId(semid);

    if(op<0){
        acquireSema(sema);
    }
    else{
        releaseSema(sema);
    }
    return 0;

    // NOT IMPLEMENTED
}