/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

/* Nanvix UNO */

#include <stdio.h>
#include <sys/sem.h>
#include <nanvix/pm.h>

#define SEMA_TABLE_SIZE 50

#define S_INVALID 0
#define S_VALID 1

typedef struct semaphore
{
    int val;
    struct process **blocked;
} semaphore_t, *pSemaphore_t;

typedef struct semCell
{
    int valid;
    unsigned key;
    semaphore_t sema;
} semCell_t, *pSemCell_t;

semCell_t semaTab[SEMA_TABLE_SIZE];

int valid(pSemCell_t sc)
{
    return sc->valid == S_VALID;
}

semaphore_t createSema(int val)
{
    return (semaphore_t){val, NULL};
}

void acquireSema(pSemaphore_t sema)
{
    if (sema->val <= 0)
    {
        enable_interrupts();
        // PUT CURRENT PROCESS TO SLEEP
        sleep(sema->blocked, curr_proc->nice);
        return;
    }

    sema->val--;
}

void releaseSema(pSemaphore_t sema)
{
    if (sema->val == 0 && sema->blocked != NULL)
    {
        // AWAKE OLDEST BLOCKED PROCESS
        //struct proces **chain_adr = &sema->blocked;
        wakeupLast(sema->blocked);
        return;
    }

    sema->val++;
}

void destroySema(pSemaphore_t sema)
{
    if (sema != NULL)
        return;
    
    while (sema->blocked != NULL)
    {   
        struct process *p = *(sema->blocked);
        *(sema->blocked) = p->next;
        p->next = NULL;
    }
}

pSemCell_t getSemCell(int semid)
{
    if (semid < 0 || semid >= SEMA_TABLE_SIZE)
        return NULL;

    return &semaTab[semid];
}

/*
 * Get the semaphore matching the key or create a new one
 */
PUBLIC int sys_semget(unsigned key)
{
    int idx = -1;

    /*parcours du tableau afin de trouver la key*/
    for (int i = 0; i < SEMA_TABLE_SIZE; i++)
    {
        if (valid(&semaTab[i]))
        {
            if (semaTab[i].key == key)
                return i;
        }
        else if (idx == -1)
            idx = i;
    }

    if (idx != -1)
    {
        semaphore_t sema = createSema(1); //par défaut on met à 1 la valeur du sémaphore
        semaTab[idx] = (semCell_t){S_VALID, key, sema};
    }

    return idx;
}

/*
 * Executes control-related operations on a semaphore
 */
PUBLIC int sys_semctl(int semid, int cmd, int val)
{
    pSemCell_t sc = getSemCell(semid);

    if (sc == NULL || !valid(sc))
        return -1;

    switch (cmd)
    {
    case GETVAL:
        return sc->sema.val;
    case SETVAL:
        sc->sema.val = val;
        return 0;
    case IPC_RMID:
        destroySema(&(sc->sema));
        sc->valid = S_INVALID;
        return 0;
    default:
        return -1;
    }
}

/*
 * Executes atomic operations on a semaphore
 */
PUBLIC int sys_semop(int semid, int op)
{
    pSemCell_t sc = getSemCell(semid);

    if (sc == NULL || !valid(sc))
        return -1;

    if (op < 0)
    {
        disable_interrupts();
        acquireSema(&(sc->sema));
        enable_interrupts();
    }
    else
    {
        disable_interrupts();
        releaseSema(&(sc->sema));
        enable_interrupts();
    }

    return 0;
}