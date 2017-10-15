/*
 * File:	timer.c
 * Purpose:
 *
 * Notes:		
 */

#include "common.h"
#include "timer.h"
/******************************************************************************/
void timer_init(void)
{
    // open the clock gate to make the PIT work
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

    // turn on PIT
    PIT_MCR = 0x00;

    // Timer 0
    PIT_LDVAL0 = 64 * periph_clk_mhz +25;//25; // setup timer 0 for 64*90 cycles, T=64uS  移动图像 向左为- 1个像素为16
    PIT_TCTRL0 = 0x00; // disable Timer 0 interrupts

    // Timer 1
    PIT_LDVAL1 = 0xFFFFFFE0; // setup timer 2 for the maximum cycles
    PIT_TCTRL1 = 0x00; // disable Timer 1 interrupts

    // Timer 2
    //PIT_LDVAL2 = 0xFFFFFFE0; // setup timer 2 for the maximum cycles
    PIT_LDVAL2 = 4000 * periph_clk_mhz;
    PIT_TCTRL2 = 0x00; // disable Timer 2 interrupts
    PIT_TCTRL2 = 0x03;
 
    // Timer 3
    PIT_LDVAL3 = 0xFFFFFFE0; // setup timer 3 for the maximum cycles
    PIT_TCTRL3 = 0x00; // disable Timer 3 interrupts

}
//void Delay_T3_mS(int n)
//{
//  for(int i=0;i<n;i++)
//  {
//	for(int j=0;j<32000;j++)
//	  ;
//  }
//}
//void Delay_T3_uS(int n)
//{
//  for(int i=0;i<n;i++)
//  {
//	for(int j=0;j<32;j++)
//	  ;
//  }
//}
/******************************************************************************/
