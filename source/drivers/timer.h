/*
 * File:	timer.h
 * Purpose:
 *
 * Notes:		
 */

#ifndef __TIMER_H__
#define __TIMER_H__

/******************************************************************************/

// Function prototypes
void timer_init(void);

// define, interface
/* To abort the current cycle and start a timer period with the new value,
 * the timer must be disabled and enabled again.
 */
#define TIME0_RESET()       {PIT_TCTRL0=0x00;PIT_TCTRL0=0x01;}//Ϊ��ͬ���ж��ṩtimer��ʱ����disable����enable
#define TIME0_GET()         (PIT_CVAL0 >> 4)

#define TIME1_RESET()       {PIT_TCTRL1=0x00;PIT_TCTRL1=0x01;}//Ϊ�ɼ������ṩtimer��ʱ����disable����enable
#define TIME1_GET_uS()      ((0xFFFFFFFFUL-PIT_CVAL1)/periph_clk_mhz)

#define TIME2_RESET()       {PIT_TCTRL2=0x00;PIT_TCTRL2=0x03;}//Ϊ�ٿ��ж�4ms�ж��ṩtimer,PIT_TCTRL2=0x00�жϡ�ʱ��disable��PIT_TCTRL2=0x03�жϡ�ʱ��enable
#define TIME2_interrupt_DIS()   PIT_TCTRL2=0x00

//#define TIME2_GET_uS()      ((0xFFFFFFFFUL-PIT_CVAL2)/periph_clk_mhz)

/*
 * This timer is used by Delay_T3_uS(), Do not use it for other purposes !!
 */
#define TIME3_RESET()       {PIT_TCTRL3=0x00;PIT_TCTRL3=0x01;}   //Ϊdelay�ṩtimer��ʱ����disable����enable
#define TIME3_GET_uS()      ((0xFFFFFFFFUL-PIT_CVAL3)/periph_clk_mhz)
//#define TIME3_GET_001uS()    ((int64)(0xFFFFFFFFUL-PIT_CVAL3)*100/periph_clk_mhz)
//extern void Delay_T3_mS(int n);
//extern void Delay_T3_uS(int n);
/******************************************************************************/
#endif /* __TIMER_H__ */
