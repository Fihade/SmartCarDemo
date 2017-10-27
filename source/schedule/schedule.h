/*
 * File:	schedule.h
 * Purpose:	schedule
 *
 * Notes:		
 */

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

/******************************************************************************/
typedef uint8   INT8U;
typedef int8    INT8S;
typedef uint16  INT16U;
typedef int16   INT16S;
typedef uint32  INT32U;
typedef int32   INT32S;
typedef uint64  INT64U;
typedef int64   INT64S;

/*----------------------- M A C R O S ----------------------------------------*/

/*
* ����ͷ����
*/
#define   START_VIDEO_LINE	        105          	//ͼ����ʼ��
#define   MAX_VIDEO_POINT               162             //ÿ�вɼ��ĵ���
#define   MID_VIDEO_POINT               81              //ͼ�����ĵ�
#define   MAX_VIDEO_LINE	        60     //ÿ��ʵ���õ�������
#define   VIDEO_PORT                GPIOC_PDIR      //����ͷ�ź�

#define   SetBit(reg,bit)		    reg |= (1 << bit)
#define   ClrBit(reg,bit)		    reg &= ~(1 << bit)

/*
* ͼ��ɼ�
*/
typedef enum        //ͼ��ɼ�״̬����״̬������״ֵ̬��Ϊö����
{
    VIDEO_WAIT,
    VIDEO_ACQUISITION,
    VIDEO_SEARCH,
    VIDEO_FINISH,
}Video_state;

typedef struct      //ͼ�����ݽṹ  
{
    int16 x;
    int16 y;
}Int16_point;

/*********ͼ����********/
#define   MAX_POINT_NUM                 80                  //���غ�����������
#define   IMAGE_MIDDLE                  170  //ͼ����ֵ�������Լ���ͼ���ʵ�����
#define   WIDTH                         60                  //��һ�а׿���Ҫ�� ����ǰհ�Լ�������ͼ������ʵ�����
#define   INIT_DATA			250                 //��Ч����

/*----------------------- D E C L A R A T I O N ------------------------------*/
extern Video_state  g_VideoFlag;


/******************************************************************************/

extern int32 g_time;
extern int g_sendflag;
extern int g_StopTimer;
/******************************************************************************/
/*----------------------- D E C L A R A T I O N ------------------------------*/
// Function prototypes
void ScheduleInit(void);
void Running(void);
void Running_Init(void);
void ScheduleInit(void);
void Search(void);
void pit_4000uS_isr(void);  
void Running_Init(void);
void Car_Run(void);
void Car_Turn(void);
void vsync_isr(void);					//���ж�
void href_isr(void);					//���ж�

#endif /* __SCHEDULE_H__ */