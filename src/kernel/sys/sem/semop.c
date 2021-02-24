#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Executes atomic operations on a semaphore
 */
PUBLIC int semop(int semid, int op)
{
    pSemaphore_t sema=getSemWithId(semid);

    if(op<0){
        disable_interrupts();
        acquireSema(sema);
        enable_interrupts();
    }
    else{
        disable_interrupts();
        releaseSema(sema);
        enable_interrupts();
    }
    return 0;

    // NOT IMPLEMENTED
}