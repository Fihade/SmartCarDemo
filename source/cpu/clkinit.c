/*
 * File:        clkinit.c
 * Purpose:     Kinetis Configuration
 *              Initializes processor to a default clock state
 *
 * Notes:
 *
 */
#include "common.h"
#include "clkinit.h"

/******************************************************************************/

/* Actual system clock frequency */
int core_clk_khz;
int core_clk_mhz;
int periph_clk_khz;
int periph_clk_mhz;

/******************************************************************************/
void clkinit (void)
{
   /*
    * Enable all of the port clocks. These have to be enabled to configure
    * pin muxing options, so most code will need all of these on anyway.
    */
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                | SIM_SCGC5_PORTB_MASK
                | SIM_SCGC5_PORTC_MASK
                | SIM_SCGC5_PORTD_MASK
                | SIM_SCGC5_PORTE_MASK);

    /* Ramp up the system clock */
	core_clk_mhz = pll_init(CORE_CLK_MHZ);

   /*
    * Use the value obtained from the pll_init function to define variables
    * for the core clock in kHz and also the peripheral clock. These
    * variables can be used by other functions that need awareness of the
    * system frequency.
    */
	core_clk_khz = core_clk_mhz * 1000;
  	periph_clk_khz = core_clk_khz
                   / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
    periph_clk_mhz = periph_clk_khz / 1000;
    
   /* For debugging purposes, enable the trace clock and/or FB_CLK so that
    * we'll be able to monitor clocks and know the PLL is at the frequency
    * that we expect.
    */
	trace_clk_init(); /* Set the trace clock to the core clock frequency */
  	fb_clk_init(); /* Enable the clock to the FlexBus module */

} // sysinit
/******************************************************************************/