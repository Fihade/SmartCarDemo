/*
 * File:		interrupt.h
 * Purpose:
 *
 * Notes:
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
/******************************************************************************/

/* ARM Cortex M4 implementation for interrupt priority shift */
#define ARM_INTERRUPT_LEVEL_BITS    4

// Define

#define INTERRUPT_VSYNC_PIN       (6)                  //IRQ
#define INTERRUPT_VSYNC_DDR       GPIOC_PDDR
#define INTERRUPT_VSYNC_PCR       PORTC_PCR6
#define INTERRUPT_VSYNC_IRQ       (89) //(61) //PTC

#define INTERRUPT_HREF_PIN      (0)                    // O/E        
#define INTERRUPT_HREF_DDR      GPIOD_PDDR//GPIOD_PDDR
#define INTERRUPT_HREF_PCR      PORTD_PCR0//PORTD_PCR0
#define INTERRUPT_HREF_IRQ      (90)//(62) // PTD



/* Set Interrupt Vector Table Function Pointers */
#undef VECTOR_106 // INTERRUPT_VSYNC_IRQ + 16
#define VECTOR_106    href_isr

#undef VECTOR_105 // INTERRUPT_HREF_IRQ +16
#define VECTOR_105   vsync_isr
   
//#define		INTERRUPT_UART_IRQ		(37)
//#undef VECTOR_053
//#define VECTOR_053  Bluetooth_Receive_isr


// Function prototypes
void enable_irq(int);
void disable_irq(int);
void set_irq_priority(int, int);
void interrupt_init(void);

// Interface
#define DISABLE_INTERRUPT_HREF()     disable_irq(INTERRUPT_HREF_IRQ)
#define ENABLE_INTERRUPT_HREF()      enable_irq(INTERRUPT_HREF_IRQ)

#define DISABLE_INTERRUPT_VSYNC()    disable_irq(INTERRUPT_VSYNC_IRQ) 
#define ENABLE_INTERRUPT_VSYNC()     enable_irq(INTERRUPT_VSYNC_IRQ)

#define CLR_FLAG_HREF()      INTERRUPT_HREF_PCR  |= PORT_PCR_ISF_MASK
#define CLR_FLAG_VSYNC()     INTERRUPT_VSYNC_PCR |= PORT_PCR_ISF_MASK
//#define CLR_FLAG_PIT2()     PIT_TFLG2|=PIT_TFLG_TIF_MASK;

#define INTERRUPT_PIT_4000uS_IRQ (70)

#undef VECTOR_086
#define VECTOR_086  pit_4000uS_isr

#define CLR_FLAG_PIT4000uS() PIT_TFLG2 = 0x01

/******************************************************************************/
#endif /* __INTERRUPT_H__ */