#include <stdint.h>
#include "bsp.h"
#include "tm4c_cmsis.h"


int main() {
     
    SYSCTL->RCGC2 |= (1U << 5);  /* enable clock for GPIOF */
    SYSCTL->GPIOHSCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_HS->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIOF_HS->DEN |= (LED_RED | LED_BLUE | LED_GREEN);

    SysTick->LOAD = SYS_CLOCK_HZ/2 -1;
    SysTick->VAL = 0;
    SysTick->CTRL = (1<<2U)|(1<<1U)|1U;
    
    __enable_irq();
//    GPIOF_HS->DATA_Bits[LED_BLUE] = LED_BLUE;
    while (1) {
     //   delay(500000);
        
    }
    //return 0;
}
