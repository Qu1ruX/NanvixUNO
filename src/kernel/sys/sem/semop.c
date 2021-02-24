#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Executes atomic operations on a semaphore
 */
PUBLIC int semop(int semid, int op)
{
    pSemCell_t sc = getSemCell(semid);

    if (sc == NULL || !sc->valid)
        return -1;

    if (op < 0)
    {
        disable_interrupts();
        acquireSema(sc->sema);
        enable_interrupts();
    }
    else
    {
        disable_interrupts();
        releaseSema(sc->sema);
        enable_interrupts();
    }
    return 0;

    // NOT IMPLEMENTED
}