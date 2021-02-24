#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Executes control-related operations on a semaphore
 */
PUBLIC int semctl(int semid, int cmd, int val)
{
    pSemaphore_t sema;

    switch (cmd)
    {
    case GETVAL:
        return sema->val;
    case SETVAL:
        sema->val = val;
        return 0;
    case IPC_RMID:
        destroySema(sema);
        return 0;
    }

    // NOT IMPLEMENTED
}