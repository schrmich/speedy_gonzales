/*
 * scheduler51.h
 *
 * Created: 31.10.2019 08:04:38
 *  Author: schreiberm
 */ 


#ifndef SCH51_H_
#define SCH51_H_

#define SCH_MAX_TASKS	10

#define ERROR_NO_ERROR 0
#define ERROR_SCH_TOO_MANY_TASKS 1
#define ERROR_SCH_CANNOT_DELETE_TASK 2

extern uint8_t taskID[SCH_MAX_TASKS];
 
extern char Error_code_G;

void scheduler_Init(void);

/*--------------------------------------------------------*-
SCH_Add_Task()
Causes a task (function) to be executed at regular
intervals, or after a user-defined delay.
-*--------------------------------------------------------*/
uint8_t schedulerAddTask(void (*pFunction)(), const uint16_t DELAY, const uint16_t PERIOD);

uint8_t schedulerDeleteTask(const uint8_t taskIndex);

/*--------------------------------------------------------*-
SCH_Dispatch_Tasks()
This is the 'dispatcher' function. When a task (function)
is due to run, SCH_Dispatch_Tasks() will run it.
This function must be called (repeatedly) from the main loop.
-*--------------------------------------------------------*/
void schedulerDispatchTasks(void);


void schedulerUpdate(void);

#endif /* SCH51_H_ */