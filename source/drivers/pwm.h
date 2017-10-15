/*
 * File:	pwm.h
 * Purpose:	pwm to contrl motor and servo
 *
 * Notes:		
 */

#ifndef __PWM_H__
#define __PWM_H__

/******************************************************************************/
// Define
#define PWM_MOT_PTR     FTM0_BASE_PTR // FTM0_BASE_PTR - FTM2_BASE_PTR
#define PWM_MOT_FTM     (0) // 0-2
#define PWM_MOTB_PCR     PORTD_PCR5// FTM0_CH5
#define PWM_MOTB_ALTn    (4) // 0-7
#define PWM_MOTB_CHANNEL (5) // right motor, channel 2
#define MOTOR_FULL_DUTY 1000          //电机PWM频率16KHz

#define PWM_SVO_PCR     PORTB_PCR18//// FTM2_CH0
#define PWM_SVO_ALTn    (3) // 0-7
#define PWM_SVO_PTR     FTM2_BASE_PTR // FTM0_BASE_PTR - FTM2_BASE_PTR
#define PWM_SVO_FTM     (2) // 0-2
#define PWM_SVO_CHANNEL (0)//(0) // when PWM_SVO_FTM=0 0-7, else 0-1               
#define TURN_SVO_FULL_DUTY 12500        //产生160Hz的PWM波

#define SET_PWM_MOTR(x) \
       {FTM_CnV_REG(PWM_MOT_PTR,PWM_MOTB_CHANNEL) = (ABS(x/2+500)) & 0xffff;\
         FTM_PWMLOAD_REG(PWM_MOT_PTR) |= FTM_PWMLOAD_LDOK_MASK;}
         

#define SET_PWM_SVO(x) \
       {FTM_CnV_REG(PWM_SVO_PTR,PWM_SVO_CHANNEL) = x & 0xffff;\
        FTM_PWMLOAD_REG(PWM_SVO_PTR) |= FTM_PWMLOAD_LDOK_MASK;}

// Function prototypes
void pwm_init(void);
void pwm_motor_init(void);
void pwm_servo_init(void);

/******************************************************************************/
#endif /* __PWM_H__ */
