#include <stdint.h>
#include "miros.h"

OSThread * volatile OS_curr; /* Pointer to the current thread */
OSThread * volatile OS_next; /* Pointer to the next thread */ 

void OSInit(void)
{
	/* Set the PendSV Interrupt priority to the lowest possible */
	*(uint32_t volatile *)0xE000ED20 |= (0xFF << 16);
}

void OSThread_start( 	OSThread *me,OSThreadHandler threadHandler,
void *stkSto,uint32_t stkSize)
{
	/* Give me the value of the stored at the beginning of the stack (the stack grows thourds the lower address) */
	uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize)/8) * 8); //Last mathmetical operation needs for memory alignment eg: 52 is not memory allign but 52 / 8 = 6 *8 = 48(which is memory allignt)
	uint32_t *stk_limit;
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
}

void OS_sched(void)
{
		if(OS_next != OS_curr)
		{	
			*(uint32_t volatile *)0xE000ED04 = (1U <<28U);
		}
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

