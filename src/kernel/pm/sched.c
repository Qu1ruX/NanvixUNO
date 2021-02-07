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

#define PRIORITY_MAX 140
#define NICE_MAX 40
#define PRIORITY_COEFF 1.0
#define NICE_COEFF 1.0

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
	struct process *p;    /* Working process.     */
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

	next = IDLE;
	next->counter = 0;
	float ratio = 0.0;
	float nextratio = 0.0;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;
		
		/*ratio and next ratio are both an average between nice and priority
		We chose to focus on both. None of the two has priority over the other.
		In order to focus on the user changes, you just need to increase NICE_COEFF 
		at the top of the program.
		*/
		ratio = ((float) PRIORITY_COEFF * ((p->priority - 40) / -PRIORITY_MAX) - (float) NICE_COEFF * (p->nice / NICE_MAX)) / -2;
		nextratio = ((float) PRIORITY_COEFF *((next->priority - 40) / -PRIORITY_MAX) - (float) NICE_COEFF * (p->nice / NICE_MAX)) / -2;

		/*process with highest ratio will be the next*/
		if (ratio < nextratio)
		{
			if (next != IDLE)
				next->counter++;
			next = p;
		}

		/*(if ratio are both the same),
		the process that has waited the longest will be the next.*/
		else if (ratio == nextratio)
		{
			if (p->counter > next->counter)
			{
				if (next != IDLE)
					next->counter++;
				next = p;
			}

			/*
         	* Increment waiting
         	* time of process.
         	*/
			else
			{
				if (p != IDLE)
					p->counter++;
			}
		}

		else
		{
			if (p != IDLE)
				p->counter++;
		}
	}
	
	/* Switch to next process. */
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}
