/*
 * File:	gpio.h
 * Purpose:
 *
 * Notes:
 */

#ifndef __GPIO_H__
#define __GPIO_H__

/******************************************************************************/

// Function prototypes
void gpio_init(void);

// Definition
/* jumper, input */
#define JM_0_PIN    (6)
#define JM_0_DIR    GPIOD_PDIR
#define JM_0_DDR    GPIOD_PDDR
#define JM_0_PCR    PORTD_PCR6

#define JM_1_PIN    (2)
#define JM_1_DIR    GPIOE_PDIR
#define JM_1_DDR    GPIOE_PDDR
#define JM_1_PCR    PORTE_PCR2

#define JM_2_PIN    (4)
#define JM_2_DIR    GPIOE_PDIR
#define JM_2_DDR    GPIOE_PDDR
#define JM_2_PCR    PORTE_PCR4

#define JM_3_PIN    (6)
#define JM_3_DIR    GPIOE_PDIR
#define JM_3_DDR    GPIOE_PDDR
#define JM_3_PCR    PORTE_PCR6


/* LED, output */
#define LED_0_PIN   (5)				//单片机板上LED
#define LED_0_DOR   GPIOC_PDOR
#define LED_0_DDR   GPIOC_PDDR
#define LED_0_PCR   PORTC_PCR5

#define LED_1_PIN   (26)
#define LED_1_DOR   GPIOE_PDOR
#define LED_1_DDR   GPIOE_PDDR
#define LED_1_PCR   PORTE_PCR26

#define LED_A_PIN   (3)				//主板上LED
#define LED_A_DOR   GPIOE_PDOR
#define LED_A_DDR   GPIOE_PDDR
#define LED_A_PCR   PORTE_PCR3

#define LED_B_PIN   (5)
#define LED_B_DOR   GPIOE_PDOR
#define LED_B_DDR   GPIOE_PDDR
#define LED_B_PCR   PORTE_PCR5

#define LED_C_PIN   (24)
#define LED_C_DOR   GPIOE_PDOR
#define LED_C_DDR   GPIOE_PDDR
#define LED_C_PCR   PORTE_PCR24

#define LED_D_PIN   (25)
#define LED_D_DOR   GPIOE_PDOR
#define LED_D_DDR   GPIOE_PDDR
#define LED_D_PCR   PORTE_PCR25

/* 键盘模块 */
#define ZLG7290_INT_PIN     (9)         //PTB9
#define ZLG7290_INT_DIR  GPIOB_PDIR
#define ZLG7290_INT_DDR  GPIOB_PDDR
#define ZLG7290_INT_PCR  PORTB_PCR9

/* Interface */
#define JM_0_STATUS()           BIT_IS_HIGH(JM_0_DIR, JM_0_PIN)
#define JM_1_STATUS()           BIT_IS_HIGH(JM_1_DIR, JM_1_PIN)
#define JM_2_STATUS()           BIT_IS_HIGH(JM_2_DIR, JM_2_PIN)
#define JM_3_STATUS()           BIT_IS_HIGH(JM_3_DIR, JM_3_PIN)
#define ZLG7290_INT()           BIT_IS_HIGH(ZLG7290_INT_DIR, ZLG7290_INT_PIN)

#define LED_0_ON()              CLR_BIT(LED_0_DOR, LED_0_PIN)
#define LED_0_OFF()             SET_BIT(LED_0_DOR, LED_0_PIN)
#define LED_0_REV()             REV_BIT(LED_0_DOR, LED_0_PIN)

#define LED_1_ON()              CLR_BIT(LED_1_DOR, LED_1_PIN)
#define LED_1_OFF()             SET_BIT(LED_1_DOR, LED_1_PIN)
#define LED_1_REV()             REV_BIT(LED_1_DOR, LED_1_PIN)

#define LED_A_ON()              CLR_BIT(LED_A_DOR, LED_A_PIN)
#define LED_A_OFF()             SET_BIT(LED_A_DOR, LED_A_PIN)
#define LED_A_REV()             REV_BIT(LED_A_DOR, LED_A_PIN)

#define LED_B_ON()              CLR_BIT(LED_B_DOR, LED_B_PIN)
#define LED_B_OFF()             SET_BIT(LED_B_DOR, LED_B_PIN)
#define LED_B_REV()             REV_BIT(LED_B_DOR, LED_B_PIN)

#define LED_C_ON()              CLR_BIT(LED_C_DOR, LED_C_PIN)
#define LED_C_OFF()             SET_BIT(LED_C_DOR, LED_C_PIN)
#define LED_C_REV()             REV_BIT(LED_C_DOR, LED_C_PIN)

#define LED_D_ON()              CLR_BIT(LED_D_DOR, LED_D_PIN)
#define LED_D_OFF()             SET_BIT(LED_D_DOR, LED_D_PIN)
#define LED_D_REV()             REV_BIT(LED_D_DOR, LED_D_PIN)


#define	SET_DEBUG_A(x)					\
		{					\
		  	if(x)	LED_A_ON();		\
			else	LED_A_OFF();		\
		}

#define	SET_DEBUG_B(x)					\
		{					\
		  	if(x)	LED_B_ON();		\
			else	LED_B_OFF();		\
		}

#define	SET_DEBUG_C(x)					\
		{					\
		  	if(x)	LED_C_ON();		\
			else	LED_C_OFF();		\
		}

#define	SET_DEBUG_D(x)					\
		{					\
		  	if(x)	LED_D_ON();		\
			else	LED_D_OFF();		\
		}

/******************************************************************************/
#endif /* __GPIO_H__ */
