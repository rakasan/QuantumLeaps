#ifndef MIROS__H__
#define MIROS__H__
/*****************************
General information regarding the file, and that this is a learning envirement
*****************************/

/* Thread control block */
typedef struct{
	void *sp; /* Stack pointer */
	/* Other atributes associatead with a thread */
	uint32_t timeout; /* Time out delay down counter*/
 
}OSThread;

typedef void (*OSThreadHandler)();

void OSThread_start(
	OSThread *me,
	OSThreadHandler threadHandler,
void *stkSto,uint32_t stkSize); /*stkSto - stack storage, stkSize - stack size */

void OSInit(void *stkSto,uint32_t stkSize);

void OS_onIdle(void);
void OS_run(void);

void OS_tick(void);

void OS_delay(uint32_t ticks);
void OS_onStartUp(void);

/* Scheduler for the threads */
/* This function must be called from a critical area */
void OS_sched(void);
#endif /*MiROS_H */
