/*
 * File:	pwm.c
 * Purpose:	PWM initialization
 *
 * Notes:		
 */
#include "common.h"
#include "pwm.h"

/******************************************************************************/
/*!
 * \brief   PWM initialization
 * \return  None
 *
 * This function initializes the PWM generator
 */
void pwm_init(void)
{
    pwm_motor_init();
    pwm_servo_init();
} // pwm_init

/******************************************************************************/
void pwm_motor_init(void) // motor
{
    // FTM0_CH5
#if(0 == PWM_MOT_FTM)
    SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; // open the clock gate to the FTM1 module
#elif(1 == PWM_MOT_FTM)
    SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
#elif(2 == PWM_MOT_FTM)
    SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
#else
#error "Definition PWM_MOT_FTM ERROR ! "
#endif

    FTM_MODE_REG(PWM_MOT_PTR) = 0|FTM_MODE_WPDIS_MASK; // Write protection is disabled
    FTM_SC_REG(PWM_MOT_PTR) = 0UL; // clear first
    FTM_CNTIN_REG(PWM_MOT_PTR) = 0UL; // counter initialization value is 0
    FTM_CNT_REG(PWM_MOT_PTR) = 0UL;

#if(0 == PWM_MOT_FTM)
    FTM_CnSC_REG(PWM_MOT_PTR,0) = 0UL; // disable the channel
    FTM_CnSC_REG(PWM_MOT_PTR,1) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,2) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,3) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,4) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,5) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,6) = 0UL;
    FTM_CnSC_REG(PWM_MOT_PTR,7) = 0UL;
#else
    FTM_CnSC_REG(PWM_MOT_PTR,0) = 0UL; // disable the channel
    FTM_CnSC_REG(PWM_MOT_PTR,1) = 0UL;
#endif

    FTM_MOD_REG(PWM_MOT_PTR) = FTM_MOD_MOD(MOTOR_FULL_DUTY); // Modulo value is 1000
    FTM_CnSC_REG(PWM_MOT_PTR,PWM_MOTB_CHANNEL) = FTM_CnSC_ELSB_MASK|FTM_CnSC_MSB_MASK; // Edge-aligned PWM
    FTM_CnV_REG(PWM_MOT_PTR,PWM_MOTB_CHANNEL) = 0UL; // Set up channel value register
    
    PWM_MOTB_PCR = 0 | PORT_PCR_MUX(PWM_MOTB_ALTn); // set PTA8 to ALT3, route the desired signal to the pin
    //PWM_MOT_PCR |= PORT_PCR_ODE_MASK; // Open Drain Enable
    FTM_SC_REG(PWM_MOT_PTR) = FTM_SC_CLKS(1)|FTM_SC_PS(2); // System/bus clock, Divide by 1(不是64k), 分频要调

    SET_PWM_MOTR(0); // set initialization of PWM to the motor

} // pwm_motor_init

/******************************************************************************/
void pwm_servo_init(void) // servo
{
    // FTM1_CH0
#if(0 == PWM_SVO_FTM)
    SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; // open the clock gate to the FTM0 module
#elif(1 == PWM_SVO_FTM)
    SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
#elif(2 == PWM_SVO_FTM)
    SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
#else
#error "Definition PWM_SVO_FTM ERROR ! "
#endif

    FTM_MODE_REG(PWM_SVO_PTR) = 0|FTM_MODE_WPDIS_MASK; // Write protection is disabled
    FTM_SC_REG(PWM_SVO_PTR) = 0UL;
    FTM_CNTIN_REG(PWM_SVO_PTR) = 0UL;  // counter initialization value is 0
    FTM_CNT_REG(PWM_SVO_PTR) = 0UL; //  write any value to CNT register

#if(0 == PWM_SVO_FTM)
    FTM_CnSC_REG(PWM_SVO_PTR,0) = 0UL; // disable the channel
    FTM_CnSC_REG(PWM_SVO_PTR,1) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,2) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,3) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,4) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,5) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,6) = 0UL;
    FTM_CnSC_REG(PWM_SVO_PTR,7) = 0UL;
#else
    FTM_CnSC_REG(PWM_SVO_PTR,0) = 0UL; // disable the channel
    FTM_CnSC_REG(PWM_SVO_PTR,1) = 0UL;
#endif

    FTM_MOD_REG(PWM_SVO_PTR) = FTM_MOD_MOD(TURN_SVO_FULL_DUTY); // Modulo value is 13000(76Hz)
    FTM_CnSC_REG(PWM_SVO_PTR,PWM_SVO_CHANNEL) = FTM_CnSC_ELSB_MASK|FTM_CnSC_MSB_MASK; // Edge-aligned PWM
    FTM_CnV_REG(PWM_SVO_PTR,PWM_SVO_CHANNEL) = 0UL;//1500UL // Set up channel value register

    PWM_SVO_PCR = 0 | PORT_PCR_MUX(PWM_SVO_ALTn); // set PTC1 to ALT4, route the desired signal to the pin
    //PWM_SVO_PCR |= PORT_PCR_ODE_MASK; // Open Drain Enable
    FTM_SC_REG(PWM_SVO_PTR) = FTM_SC_CLKS(1)|FTM_SC_PS(5);//6 // System/bus clock, Divide by 32 //64

    SET_PWM_SVO(PWM_SVO_MIDDLE); // set initialization of PWM to the servo
  
} // pwm_servo_init

/******************************************************************************/
