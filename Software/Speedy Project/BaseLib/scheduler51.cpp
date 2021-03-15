/*
 * scheduler51.c
 *
 * Created: 31.10.2019 08:04:51
 *  Author: schreiberm
 */ 

#include <avr/interrupt.h>
#include "scheduler51.h"

/* Store in DATA area, if possible, for rapid access
Total memory per task is 7 bytes */
typedef struct
{
	/* Pointer to the task (must be a 'void (void)' function) */
	void (*pTask)(void);
	/* Delay (ticks) until the function will (next) be run
	- see SCH_Add_Task() for further details */
	unsigned int Delay;
	/* Interval (ticks) between subsequent runs.
	- see SCH_Add_Task() for further details */
	unsigned int Period;
	/* Incremented (by scheduler) when task is due to execute */
	char RunMe;
} sTask_t;


/* The array of tasks */
sTask_t schTasks_G[SCH_MAX_TASKS];

Scheduler51::Scheduler51()
{
	for (uint8_t i=0; i<SCH_MAX_TASKS; i++)
	{
		deleteTask(i);
	}
	
	/* schedulerDeleteTask() will generate an error code,
	because the task array is empty.
	-> reset the global error variable. */
	Error_code_G = 0;
}

/*--------------------------------------------------------*-
schedulerAddTask()
Causes a task (function) to be executed at regular
intervals, or after a user-defined delay.
-*--------------------------------------------------------*/
uint8_t Scheduler51::addTask(void (*pFunction)(), const uint16_t DELAY, const uint16_t PERIOD)
{
	uint8_t Index = 0;
	/* First find a gap in the array (if there is one) */
	while ((schTasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))
	{
		Index++;
	}
	
	/* Have we reached the end of the list? */
	if (Index == SCH_MAX_TASKS)
	{
		/* Task list is full
		-> set the global error variable */
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
		/* Also return an error code */
		return SCH_MAX_TASKS;
	}
	
	/* If we're here, there is a space in the task array */
	schTasks_G[Index].pTask = pFunction;
	
	schTasks_G[Index].Delay = DELAY + 1;
	schTasks_G[Index].Period = PERIOD;
	
	schTasks_G[Index].RunMe = 0;
	
	return Index; /* return pos. of task (to allow deletion) */
}

uint8_t Scheduler51::deleteTask(const uint8_t taskIndex)
{
	uint8_t Return_code;
	
	if(taskIndex > SCH_MAX_TASKS-1)
	{
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
		return 1;
	}
	
	if (schTasks_G[taskIndex].pTask == 0)
	{
		/* No task at this location...
		-> set the global error variable */
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
		/* ...also return an error code */
		Return_code = 1;
	}
	else
	{
		Return_code = 0;
	}
	
	schTasks_G[taskIndex].pTask = 0x0000;
	schTasks_G[taskIndex].Delay = 0;
	schTasks_G[taskIndex].Period = 0;
	
	schTasks_G[taskIndex].RunMe = 0;
	
	return Return_code; /* return status */
}

/*--------------------------------------------------------*-
SCH_Dispatch_Tasks()
This is the 'dispatcher' function. When a task (function)
is due to run, SCH_Dispatch_Tasks() will run it.
This function must be called (repeatedly) from the main loop.
-*--------------------------------------------------------*/
void Scheduler51::dispatchTasks(void)
{
	uint8_t Index;
	/* Dispatches (runs) the next task (if one is ready) */
	for (Index = 0; Index < SCH_MAX_TASKS; Index++)
	{
		if (schTasks_G[Index].RunMe > 0)
		{
			(*schTasks_G[Index].pTask)(); /* Run the task */
			schTasks_G[Index].RunMe -= 1; /* Reduce RunMe count */
			/* Periodic tasks will automatically run again
			- if this is a 'one shot' task, delete it */
			if (schTasks_G[Index].Period == 0)
			{
				deleteTask(Index);
			}
		}
	}
}

/*--------------------------------------------------------*/
void Scheduler51::update(void) 
{
	
	/* NOTE: calculations are in *TICKS* (not milliseconds) */
	for (uint8_t Index = 0; Index < SCH_MAX_TASKS; Index++)
	{
		/* Check if there is a task at this location */
		if (schTasks_G[Index].pTask)
		{
			// Check if the Task is ready to execute but not executed yet
			if (schTasks_G[Index].RunMe == 0)
			{
				// Decrement Delay and check if task is ready to execute
				if (--schTasks_G[Index].Delay == 0)
				{
					/* The task is due to run */
					schTasks_G[Index].RunMe += 1; /* Inc. 'RunMe' flag */
					if (schTasks_G[Index].Period)
					{
						/* Schedule regular tasks to run again */
						schTasks_G[Index].Delay = schTasks_G[Index].Period;
					}
				}
			}
			
		}
	}
}
