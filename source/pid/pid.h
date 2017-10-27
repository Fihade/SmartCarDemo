/********************************************************************
**	Module Name: PID.h
**	Module Date: 21/09/2014
**	Module Auth: Haoyu Tang
**  Description: caculate PID
**--------------------------------------------------------------------
**  Revision History:
**  Date:		month/day/year
**  Notes:
** ===================================================================
**********************************************************************/
#ifndef __PID_H__
#define __PID_H__

/*********PID参数*********/
/* 电机速控 */
#define MOTOR_PID_MAX         (1000)       //电机PID输出上限
#define MOTOR_PID_MIN         (-1000)      //电机PID输出下限

/* 舵机转向 */
#define PWM_SVO_MAX     9548         //左转的最大值
#define PWM_SVO_MIN     8880       //右转的最大值
#define PWM_SVO_MIDDLE  9245       //舵机中值
#define TURN_Amplitude  MAX(PWM_SVO_MAX-PWM_SVO_MIDDLE,PWM_SVO_MIDDLE-PWM_SVO_MIN)

#define TURN_PID_MAX         (TURN_Amplitude)  
#define TURN_PID_MIN         (-TURN_Amplitude)     

/*PID死区*/
#define VV_DEADLINE 1	//速度PID，设置死区范围

typedef struct PID			//定义数法核心数据
{
    int32 vi_Ref;		//速度PID，速度设定值
    int32 vi_FeedBack;		//速度PID，速度反馈值

    int32 vi_PreError;	  	//速度PID，前一次，速度误差,,vi_Ref - vi_FeedBack
    int32 vi_PreDerror;	//速度PID，前一次，速度误差之差，d_error-PreDerror;

    float v_Kp;		//速度PID，Ka = Kp
    float v_Ki;		//速度PID，Kb = Kp * ( T / Ti )
    float v_Kd;		//速度PID，

    int32 v_MAX;
    int32 v_MIN;

    int32 servo_PreU;  //控制输出
    int32 motor_PreU;

} PID;

extern PID motorPID;
extern PID turnPID;

//转向参数
extern float g_turn_p;
extern float g_turn_i; 
extern float g_turn_d;

//速度控制参数
extern float g_speed_p;
extern float g_speed_i;
extern float g_speed_d;

extern int v_PIDCalc(PID *pp);
extern void Motor_PIDInit(void);
extern void Turn_PIDInit(void);
extern void PID_Init(void);

#endif