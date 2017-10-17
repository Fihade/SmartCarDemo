/*
 * File:	systeminit.c
 * Purpose: system initialization
 *
 * Notes:		
 */
#include "common.h"
#include "systeminit.h"

/******************************************************************************/
void system_init(void)
{ 
    gpio_init();
    pwm_init();
    timer_init();
    interrupt_init();
    pulse_counter_init();
    uart_init();
    i2c_init();
    i2c_init_add();
    Delay_T3_mS(500);

//    if(sccb_init()==0)
//    {
//      while(1);     
//    }
 
} // system_init
/******************************************************************************/
