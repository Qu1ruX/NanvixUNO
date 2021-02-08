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

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>

#define MAX_NICE 40
#define QUEUES 4
#define MAX_PPQ 100 /* Max number of processes per queue */

int UNO_pow(int x, int y);

/**
 * @brief Schedules a process to execution.
 * 
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 * 
 * @param proc Process to be resumed.
 * 
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;	  /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	struct process *queues[QUEUES][MAX_PPQ];
	int procPerQueue[QUEUES]; /* Number of processes per queue */

	/* Empty the queues */
	for (int i = 0; i < QUEUES; i++)
		procPerQueue[i] = 0;

	// /* Choosing the appropriate queue for each process */
	// for (p = FIRST_PROC; p <= LAST_PROC; p++)
	// {
	// 	/* Skip non-ready process. */
	// 	if (p->state != PROC_READY)
	// 		continue;

	// 	/* Choose desired queue q for process p */
	// 	int q = (p->nice / (MAX_NICE / QUEUES));
	// 	if (q >= QUEUES)
	// 		q = QUEUES - 1;

	// 	/* Put the process in the chosen queue */
	// 	queues[q][procPerQueue[q]++] = p;
	// }

	/* Multiple-Level Queues (Aging method) */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/* Put the processes in a specific queue depending on their current kernel time */
		for (int i = 0; i < QUEUES; i++)
		{
			if (i == QUEUES - 1 || p->ktime <= (unsigned int) UNO_pow(2, i) * PROC_QUANTUM)
			{
				queues[i][procPerQueue[i]++] = p;
				break;
			}
		}
	}

	/* Choose a process to run next (Fixed priority preemptive scheduling) */
	int isEmpty = 1;
	next = IDLE;
	for (int i = 0; i < QUEUES; i++)
	{
		for (int j = 0; j < procPerQueue[i]; j++)
		{
			isEmpty = 0;
			struct process *p = queues[i][j];

			/*
			* Process with higher
			* waiting time found.
			*/
			if (p->counter > next->counter)
			{
				next->counter++;
				next = p;
			}

			/*
			* Increment waiting
			* time of process.
			*/
			else
				p->counter++;
		}
		if (!isEmpty)
			break;
	}

	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

/**
 * @brief Elevates x to the power of y
 * 
 * @param x Number to elevate.
 * @param y Exponent.
 */
inline int UNO_pow(int x, int y)
{
	if (y == 0) return 1;	
	else return x * UNO_pow(x, y - 1);
}