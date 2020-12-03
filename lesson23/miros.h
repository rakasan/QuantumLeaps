#ifndef MIROS__H__
#define MIROS__H__
/*****************************
General information regarding the file, and that this is a learning envirement
*****************************/

/* Thread control block */
typedef struct{
	void *sp; /* Stack pointer */
	/* Other atributes associatead with a thread */
}OSThred;

typedef void (*OSThreadHandler)();

void OSThread_start(
	OSThred *me,
	OSThreadHandler threadHandler,
void *stkSto,uint32_t stkSize); /*stkSto - stack storage, stkSize - stack size */

void OSInit(void);
#endif /*MiROS_H */
