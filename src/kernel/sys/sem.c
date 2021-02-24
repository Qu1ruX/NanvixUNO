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

#include "include/nanvix/pm.h"

#define SEMA_TABLE_SIZE 50

#define SV_FALSE 0
#define SV_TRUE  1

// MAKE SURE TO INITIALIZE EACH CELL!!!!!
static pSemCell_t semaTab[SEMA_TABLE_SIZE];

typedef struct semCell
{
    int valid;
    unsigned key;
    pSemaphore_t sema;
} semCell_t, *pSemCell_t;

typedef struct semaphore
{
    int val;
    struct process *blocked;
} semaphore_t, *pSemaphore_t;

pSemaphore_t createSema(int val)
{
    return (pSemaphore_t){val, NULL};
}

void acquireSema(pSemaphore_t sema)
{
    if (sema->val <= 0)
    {
        enable_interrupts();
        // PUT CURRENT PROCESS TO SLEEP
        sleep(curr_proc, curr_proc->nice);
        curr_proc->nextBlocked = sema->blocked;
        sema->blocked = curr_proc;
        return;
    }

    sema->val--;
}

inline void releaseSema(pSemaphore_t sema)
{
    if (sema->val == 0 && sema->blocked != NULL)
    {
        // AWAKE OLDEST BLOCKED PROCESS
        struct process *awakenProc = sema->blocked;
        sema->blocked = awakenProc->nextBlocked;
        awakenProc->nextBlocked = NULL;
        wakeup(awakenProc);
        return;
    }

    sema->val++;
}

void destroySema(pSemaphore_t sema)
{
    while (sema->blocked != NULL)
    {
        struct process *p = sema->blocked;
        sema->blocked = p->nextBlocked;
        p->nextBlocked = NULL;
    }
}

pSemCell_t getSemCell(int semid)
{
    if (semid < 0 || semid >= SEMA_TABLE_SIZE)
        return NULL;

    return semaTab[semid];
}

/*
valid
key
semaphoreStruct
*/