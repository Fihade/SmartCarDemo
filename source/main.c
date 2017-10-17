/*
 * File:	main.c
 * Purpose: the entrance of the program
  *
 * Notes:		
 */
#include "common.h"

/******************************************************************************/
int servo_test=PWM_SVO_MIDDLE;
int motor_test=0;
void main(void)
{

    DisableInterrupts();//关中断

    system_init();//初始化

    ScheduleInit();//初始化

    EnableInterrupts();//开中断

    for(;;)
    {

      //SET_PWM_SVO(servo_test);
      //SET_PWM_MOTR(motor_test);
      if(g_VideoFlag == VIDEO_SEARCH)
      {
        Running();
      }

    }

} // main


/******************************************************************************/
