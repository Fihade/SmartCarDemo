/*
 * File:        common.h
 * Purpose:     File to be included by all project files
 *
 * Notes:
 */
#ifndef __COMMON_H__
#define __COMMON_H__

/******************************************************************************/

/*
 * Debug prints ON (#define) or OFF (#undef)
 */
#define DEBUG
#define DEBUG_PRINT

/*
 * Conditional compilation config
 */
#define PRINT_EN    0 // function printf() 1


/*
 * Config Defines
 */
#define CORE_CLK_MHZ    PLL128
#define UART_BAUD  115200         // UART baud rate


/*
 * Defines used as function
 */
#define Delay_T3_uS(n)  {TIME3_RESET();while(TIME3_GET_uS()<=n);}
#define Delay_T3_mS(n)  {TIME3_RESET();while(TIME3_GET_uS()<=n*1000);}

/*
 * Include the cpu specific header file
 */
#include<freescale/MK60DZ10.h> // pin100
#include<c/intrinsics.h>

/*
 * Include the generic CPU header file
 */
#include "clkinit.h"
#include "core.h"
#include "start.h"
#include "vectors.h"
#include "stdio.h"

/*
 * Include drivers utilities
 */
#include "gpio.h"
#include "interrupt.h"
#include "mcg.h"
#include "pulse_counter.h"
#include "pwm.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "wdog.h"
#include "adc.h"
#include "sccb.h"
#include "pid.h"
   
#include "keyboard.h"
#include "i2c.h"
#include "keyboard_oled.h"
#include "i2chard.h"
#include "3IN1.h"

/*
 * Include common utilities
 */
#if(1==PRINT_EN)
    #include "print.h"
#endif // #if(1==PRINT_EN)

#include "systeminit.h"
#include "schedule.h"


/******************************************************************************/

#endif /* __COMMON_H__ */
