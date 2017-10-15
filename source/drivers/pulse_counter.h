/*
 * File:	pulse_counter.h
 * Purpose:	pulse counter
 *
 * Notes:		
 */

#ifndef __PULSE_COUNTER_H__
#define __PULSE_COUNTER_H__

/********************************************************************/

// Function prototypes
#define PULSE_COUNTER_FTM       (1)
#define PULSE_COUNTER_PTR       FTM1_BASE_PTR

#define PULSE_COUNTER_QDA_PCR   PORTB_PCR0
#define PULSE_COUNTER_QDB_PCR   PORTB_PCR1
#define PULSE_COUNTER_ALTn      (6)

// interface
#define PULSE_COUNTER_GET(x) \
       {x = (int16)FTM_CNT_REG(PULSE_COUNTER_PTR); \
        FTM_CNT_REG(PULSE_COUNTER_PTR) = 0;} // Any write to CNT resets the FTM counter to the value in the CNTIN register.

void pulse_counter_init(void);


/********************************************************************/
#endif /* __PULSE_COUNTER_H__ */
