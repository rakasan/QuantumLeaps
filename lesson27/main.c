#include <stdint.h>
#include "qpc.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE
uint32_t stack_blinky1[40];
QXThread blinky1;
void main_blinky1(QXThread * const me) {
    while (1) {
        BSP_ledGreenOn();
        QXThread_delay(BSP_TICKS_PER_SEC / 4U);
        BSP_ledGreenOff();
        QXThread_delay(BSP_TICKS_PER_SEC * 3U / 4U);
    }
}

uint32_t stack_blinky2[40];
QXThread blinky2;	
void main_blinky2(QXThread * const me) {
    while (1) {
        BSP_ledBlueOn();
        QXThread_delay(BSP_TICKS_PER_SEC / 2U);
        BSP_ledBlueOff();
        QXThread_delay(BSP_TICKS_PER_SEC / 3U);
    }
}

uint32_t stack_blinky3[40];
QXThread blinky3;	
void main_blinky3(QXThread * const me) {
    while (1) {
        BSP_ledRedOn();
        QXThread_delay(BSP_TICKS_PER_SEC / 3U);
        BSP_ledRedOff();
        QXThread_delay(BSP_TICKS_PER_SEC *3U / 5U);
    }
}

/* background code: sequential with blocking version */
int main() {
    BSP_init();
		
		QF_init();
	QXThread_ctor(&blinky1,&main_blinky1,0);
	QXTHREAD_START(&blinky1,
									5,
									(void *)0,0,
								stack_blinky1,sizeof(stack_blinky1),
									(void *)0);

	QXThread_ctor(&blinky2,&main_blinky2,0);
	QXTHREAD_START(&blinky2,
									6,
									(void *)0,0,
								stack_blinky2,sizeof(stack_blinky2),
									(void *)0);

	//QXThread_ctor(&blinky3,&main_blinky3,0);
	//QXTHREAD_START(&blinky3,
	//								5,
	//								(void *)0,0,
	//							stack_blinky3,sizeof(stack_blinky3),
	//								(void *)0);
		
		/* Pass the control to the threads */
		QF_run();

    //return 0;
}
