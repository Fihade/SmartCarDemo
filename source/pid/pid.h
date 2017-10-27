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

/*********PID����*********/
/* ����ٿ� */
#define MOTOR_PID_MAX         (1000)       //���PID�������
#define MOTOR_PID_MIN         (-1000)      //���PID�������

/* ���ת�� */
#define PWM_SVO_MAX     9548         //��ת�����ֵ
#define PWM_SVO_MIN     8880       //��ת�����ֵ
#define PWM_SVO_MIDDLE  9245       //�����ֵ
#define TURN_Amplitude  MAX(PWM_SVO_MAX-PWM_SVO_MIDDLE,PWM_SVO_MIDDLE-PWM_SVO_MIN)

#define TURN_PID_MAX         (TURN_Amplitude)  
#define TURN_PID_MIN         (-TURN_Amplitude)     

/*PID����*/
#define VV_DEADLINE 1	//�ٶ�PID������������Χ

typedef struct PID			//����������������
{
    int32 vi_Ref;		//�ٶ�PID���ٶ��趨ֵ
    int32 vi_FeedBack;		//�ٶ�PID���ٶȷ���ֵ

    int32 vi_PreError;	  	//�ٶ�PID��ǰһ�Σ��ٶ����,,vi_Ref - vi_FeedBack
    int32 vi_PreDerror;	//�ٶ�PID��ǰһ�Σ��ٶ����֮�d_error-PreDerror;

    float v_Kp;		//�ٶ�PID��Ka = Kp
    float v_Ki;		//�ٶ�PID��Kb = Kp * ( T / Ti )
    float v_Kd;		//�ٶ�PID��

    int32 v_MAX;
    int32 v_MIN;

    int32 servo_PreU;  //�������
    int32 motor_PreU;

} PID;

extern PID motorPID;
extern PID turnPID;

//ת�����
extern float g_turn_p;
extern float g_turn_i; 
extern float g_turn_d;

//�ٶȿ��Ʋ���
extern float g_speed_p;
extern float g_speed_i;
extern float g_speed_d;

extern int v_PIDCalc(PID *pp);
extern void Motor_PIDInit(void);
extern void Turn_PIDInit(void);
extern void PID_Init(void);

#endif