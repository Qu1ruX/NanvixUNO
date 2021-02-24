#include "src/kernel/sys/sem.c"
#include "include/sys/sem.h"

/*
 * Get the semaphore matching the key or create a new one
 */
PUBLIC int semget(unsigned key)
{
    /*parcours du tableau afin de trouver la key*/
    for (int i = 0; i < SEMA_TABLE_SIZE; i++)
    {
        if (semaTab[i] != NULL)
        {
            if (semaTab[i]->key == key)
                return i;
        }
    }

    for (int i = 0; i < SEMA_TABLE_SIZE; i++) {
        if (semaTab[i] == NULL) {
            semaTab[i] = (pSemaphore_t){key, 1, NULL}; //par défaut on met à 1 la valeur du sémaphore
            return semaTab[i];
        }
    }

    return -1;
}