#ifndef MIROS__H__
#define MIROS__H__
/*****************************
General information regarding the file, and that this is a learning envirement
*****************************/

/* Thread control block */
typedef struct{
	void *sp; /* Stack pointer */
	/* Other atributes associatead with a thread */
}OSThread;

typedef void (*OSThreadHandler)();

void OSThread_start(
	OSThread *me,
	OSThreadHandler threadHandler,
void *stkSto,uint32_t stkSize); /*stkSto - stack storage, stkSize - stack size */

void OSInit(void);

void OS_run(void);

void OS_onStartUp(void);

/* Scheduler for the threads */
/* This function must be called from a critical area */
void OS_sched(void);
#endif /*MiROS_H */
