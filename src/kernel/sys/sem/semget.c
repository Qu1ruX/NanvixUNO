#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Get the semaphore matching the key or create a new one
 */
PUBLIC int semget(unsigned key)
{
    int idx = -1;

    /*parcours du tableau afin de trouver la key*/
    for (int i = 0; i < SEMA_TABLE_SIZE; i++)
    {
        if (semaTab[i]->valid)
        {
            if (semaTab[i]->key == key)
                return i;
        }
        else
            idx = i;
    }

    if (idx != -1) {
        pSemaphore_t sema = createSema(1); //par défaut on met à 1 la valeur du sémaphore
        semaTab[idx] = (pSemCell_t){key, sema, SV_TRUE};
    }
    
    return idx;
}