#include <stdint.h>
#include "miros.h"
#include "qassert.h"

Q_DEFINE_THIS_FILE

OSThread * volatile OS_curr; /* Pointer to the current thread */
OSThread * volatile OS_next; /* Pointer to the next thread */ 

#define MAX_THREAD 32;

#define LOG2(x) (32 - __clz(x))
OSThread *OS_thread[32+1];
uint32_t OS_readySet;

uint32_t OS_delayedSet;
OSThread IdleThread;
void main_IdleThread() {
    while (1) {
			OS_onIdle();
    }
}

void OSInit(void *stkSto,uint32_t stkSize)
{
	/* Set the PendSV Interrupt priority to the lowest possible */
	*(uint32_t volatile *)0xE000ED20 |= (0xFF << 16);
	
	OSThread_start(&IdleThread,0,&main_IdleThread,
								stkSto,stkSize);

}

void OSThread_start( 	OSThread *me,uint8_t prio,OSThreadHandler threadHandler,
void *stkSto,uint32_t stkSize)
{
	/* Give me the value of the stored at the beginning of the stack (the stack grows thourds the lower address) */
	uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize)/8) * 8); //Last mathmetical operation needs for memory alignment eg: 52 is not memory allign but 52 / 8 = 6 *8 = 48(which is memory allignt)
	uint32_t *stk_limit;
	
		Q_REQUIRE((prio < Q_DIM(OS_thread))&&(OS_thread[prio] ==(OSThread*)0U));
	
	/* fabricate Cortex-M ISR stack frame for blinky1 */
    *(--sp) = (1U << 24);  /* xPSR */
    *(--sp) = (uint32_t)threadHandler; /* PC */
    *(--sp) = 0x0000000EU; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */
    /* additionally, fake registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */
	
	/* Save the top of the stack in the threads atribute  */
	me->sp = sp;
	
	/* */
	stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1)/8)+ 1) * 8);
	
	/* Fill the stack with DEADBEEF in order to see how big the stack usage is */
	for(sp = sp-1U;sp>=stk_limit;--sp)
	{
		*sp = 0xDEADBEEF;
	}
	

	OS_thread[prio] = me;
	
	me->prio = prio;
	/*Ignore the idle thread */
	if(prio > 0)
	{
		OS_readySet |= (1U<<(prio -1U));
	}
	
	
	
}

void OS_run(void){
	OS_onStartUp();
	
	/* Start the threads  */
		 __disable_irq();
		OS_sched();
	  __enable_irq();
	
	/* This code should never be executed */
	Q_ERROR();
}

void OS_sched(void)
{
	/* Verify if no thread is ready to do something */
	if(OS_readySet == 0)
	{
		//OS_currIdx = 0;/* Index of idle thread */
		OS_next = OS_thread[0];
	}
	else
	{
		OS_next = OS_thread[LOG2(OS_readySet)];
		Q_ASSERT(OS_next != (OSThread *)0);
	}

	if(OS_next != OS_curr)
	{	
		*(uint32_t volatile *)0xE000ED04 = (1U <<28U);
	}

}

void OS_tick(void)
{
	    uint32_t workingSet = OS_delayedSet;
    while (workingSet != 0U) {
        OSThread *t = OS_thread[LOG2(workingSet)];
        uint32_t bit;
        Q_ASSERT((t != (OSThread *)0) && (t->timeout != 0U));

        bit = (1U << (t->prio - 1U));
        --t->timeout;
        if (t->timeout == 0U) {
            OS_readySet   |= bit;  /* insert to set */
            OS_delayedSet &= ~bit; /* remove from set */
        }
        workingSet &= ~bit; /* remove from working set */
    }
}

void OS_delay(uint32_t ticks) {
   	 uint32_t bit;
    __disable_irq();

    /* never call OS_delay from the idleThread */
    Q_REQUIRE(OS_curr != OS_thread[0]);

    OS_curr->timeout = ticks;
    bit = (1U << (OS_curr->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
    OS_sched();
    __enable_irq();
}

__asm
void PendSV_Handler(void) {
    IMPORT  OS_curr  /* extern variable */
    IMPORT  OS_next  /* extern variable */

    /* Disable IRQ */
    CPSID         I
        /*   if(OS_curr != (OSThread *)0) { */  /* Save the SP register into the private sp data member */   
    LDR           r1,=OS_curr /*@0x000005E8*/
    LDR           r1,[r1,#0x00]
    CBZ           r1,PendSV_restore
    /* Push the registers R4-R11 on the stack */ 
    PUSH					{r4-r11}

    LDR           r1,=OS_curr
    LDR           r1,[r1,#0x00]
    /*OS_curr->sp = sp;  */  
    STR           sp,[r1,#0x00]

PendSV_restore
    /*        sp = OS_next->sp; */  /*Save the stack context of the current thread */ 
    LDR           r1,=OS_next 
    LDR           r1,[r1,#0x00]
    LDR           sp,[r1,#0x00]
        /*   OS_curr = OS_next; */    
    LDR           r1,=OS_next 
    LDR           r1,[r1,#0x00]
    LDR           r2,=OS_curr
    STR           r1,[r2,#0x00]
    /* pop registers r4-r11 */ 
    POP						{r4-r11}
    /*          __enable_irq(); */
    CPSIE         I
    BX            lr
}

