#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Executes control-related operations on a semaphore
 */
PUBLIC int sys_semctl(int semid, int cmd, int val)
{
    pSemCell_t sc = getSemCell(semid);

    if (sc == NULL || !sc->valid)
        return -1;

    switch (cmd)
    {
    case GETVAL:
        return sc->sema->val;
    case SETVAL:
        sc->sema->val = val;
        return 0;
    case IPC_RMID:
        destroySema(sc->sema);
        semaTab[semid]->valid = SV_FALSE;
        return 0;
    }
}