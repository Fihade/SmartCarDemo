/*
 * File:        clkinit.h
 * Purpose:     Kinetis Configuration
 *              Initializes processor to a default clock state
 *
 * Notes:
 *
 */

#ifndef __CLKINIT_H__
#define __CLKINIT_H__

/******************************************************************************/

// function prototypes
void clkinit(void);


extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;
extern int periph_clk_mhz;

/******************************************************************************/
#endif /* __CLKINIT_H__ */
