/*
 * File:	schedule.c
 * Purpose:
 *
 * Notes:
 */
#include "common.h"
#include "schedule.h"
/******************************************************************************/
/******************************D E F I N I T I O N ****************************/
//=============================== ���ݲɼ� ================================//
int          g_Line;                                                    //����ͷ����
int          g_ImageLine;                                               //ͼ����������
Video_state  g_VideoFlag = VIDEO_WAIT;                                  //����ͼ��״̬��־
int          g_Display;                                                 //���� ϵͳ��ʱ
uint8        *p = NULL;                                                 //ͼ��ɼ���ָ��
int          g_LineToDiv;                                               //���вɼ��ó���
int          img_Cnt;
//=============================== ͼ������ ================================//
Int16_point  g_CenterPosition[MAX_POINT_NUM];                           //������
Int16_point  g_LeftEdge[MAX_POINT_NUM], g_RightEdge[MAX_POINT_NUM];     //���ұ���
uint8 	     video_ori[MAX_VIDEO_LINE][MAX_VIDEO_POINT] = {0};
int          g_CenterNum;                                               //���ĵ����
int          g_LeftEdgeNum, g_RightEdgeNum;                             //���ұ��ص����
INT32U Temp = 0;                                                        //�ɼ��û������
//============================== ͼ�����ñ��� ==========================//
uint16       g_Start[10], g_End[10], g_SEnum, g_BEnum;                           //�׿���㡢�׿��յ㡢�׿������ �ڿ����
uint8        g_SearchFlag;
int          g_Cover;
uint8        g_CoverIndex[10],g_CoverNum;

//==============================�����ñ���==============================//
//������Ʊ���
int     g_DirectionControl = MID_VIDEO_POINT;
int     g_FormerDirectionControl = MID_VIDEO_POINT;
int     g_DirectionControlWhole;
int     g_DirectionControlLine;
int     g_DirectionError;
int     last_g_DirectionError;
int     g_PwmServo;
int     g_speed_final;
float     turn_p = 0.0;
float     turn_d = 0.0;
//������Ʊ���
int     g_pulse;                                                       //�ջ�PID���Ƶķ���ֵ
int     g_speed_feedback;
int     g_PwmMotor;
//==============================���ݴ���===============================//
INT8U   g_SendPic = 0;                                                //���Ҫ�ô�����λ����ͼ����Ѹñ�����һ�����鿴ͼ��ʱ�Ѷ���͵���رգ�������������������������
INT16U  g_SendIndex;

INT8U  Str_ZhiDao;
INT8U  Str_YouWan;
INT8U  Str_ZuoWan;

    int Right_n = 0;
    int Left_n = 0;
    int Cross = 0;
    int Move_Edg_L ;
    int Move_Edg_R;



/*****************************************************************************/
/******************************F U N C T I O N S  *****************************/
/******************************************************************************/
/*
* Schedule�б�����ʼ��
*/
void ScheduleInit(void)
{
    Running_Init();               //���в�����ʼ��
    PID_Init();                   //PID��ʼ��
                  
    //Choise();                   //���̺�������ʱ����
    
    g_Line = 0;
    
    TIME2_interrupt_DIS(); 	 //����жϷ������ر�,PIT2  
    TIME2_RESET();
    
    ENABLE_INTERRUPT_HREF();
    ENABLE_INTERRUPT_VSYNC();
}
/*
function name:  running_init
description:    init
*/
void Running_Init(void)
{   
    //���뿪��

  g_speed_p = 150;                //���pid 
  g_speed_i = 70;
  g_speed_d = 1;
  
  
  if(JM_0_STATUS())//1 ����
  {
    LED_A_ON();    
    turn_p = 7;                  //���pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //���pid d       1     1    1.2     1.2         1        1       1    
    g_speed_final = 40;          //����ٶȸ�ֵ
  }
  
  else if(JM_1_STATUS())
  {
    LED_B_ON(); 
    turn_p = 7.4;                  //���pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.5;                    //���pid d       1     1    1.2     1.2         1        1       1    
    g_speed_final = 40;          //����ٶȸ�ֵ
  }
  
  else if(JM_2_STATUS())
  {
    LED_C_ON();   
    turn_p = 7.4;                  //���pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //���pid d       1     1    1.2     1.2         1        1       1   
    g_speed_final = 40;          //����ٶȸ�ֵ
  }
  
  else  if(JM_3_STATUS())
  {
    //
    LED_D_ON();   
    turn_p = 7.6;                  //���pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //���pid d       1     1    1.2     1.2         1        1       1  
    g_speed_final = 40;          //����ٶȸ�ֵ
  }
  else
  {
    turn_p = 7.8;                  //���pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //���pid d       1     1    1.2     1.2         1        1       1  
    g_speed_final = 40;          //����ٶȸ�ֵ
  }
  
}

/******************************************************************************/
/*
* ��̨������
*/
void Running(void)
{
  //============================�ɼ�����������==============================//  
  Search();
  
}


/*
* vsync_isr���жϷ�������
*/
void vsync_isr(void)
{
  CLR_FLAG_VSYNC();  	        //������жϱ�־���������ж�
  
  g_Line = 0;
  
  ENABLE_INTERRUPT_HREF();   //�������жϣ����жϼ���60��֮�󻻳�
  
  // ������
  if(img_Cnt %5 ==0)
  {
    LED_0_REV();
  }
  img_Cnt++;
}

/*
* href_isr���жϷ�����
*/
void href_isr(void)
{
  CLR_FLAG_HREF();	
  
  //=================================���вɼ�===============================// 
  g_Line++;
  
  if (VIDEO_WAIT == g_VideoFlag)
  {
    if (g_Line >= START_VIDEO_LINE)
    {
      g_VideoFlag = VIDEO_ACQUISITION;
      g_ImageLine = 0;
      g_LineToDiv = 3;
    }
  }
  //���ݲɼ�״̬
  else if (VIDEO_ACQUISITION == g_VideoFlag)
  {
    if (g_ImageLine >= 20 && g_LineToDiv == 3)   //���вɼ���ʽ��������20���Ǹ�1�в�һ�� ����40���Ǹ�2�в�һ��
      g_LineToDiv = 2;
    if(g_Line % g_LineToDiv == 0)
    {          
      int i;
      Delay_T3_uS(6.5);
      p = video_ori[g_ImageLine];
      
      for(i = 0; i < MAX_VIDEO_POINT; i++)   //����ʽͼ��ɼ�
      {
        Temp = (VIDEO_PORT)>>8;
        p[i] = (uint8)Temp;
        for (int m = 0; m < 3; m ++);
        asm("nop");asm("nop");asm("nop");
        //����Ҳ����д����g_VideoImageData[g_VideoImageLine][i]=(VIDEO_PORT);������Լ�����������д��������
      }
      
            g_ImageLine ++;
        }
        if(g_ImageLine >= MAX_VIDEO_LINE)
        {
            g_VideoFlag = VIDEO_SEARCH;         
        }
    }
    //ͼ����Ϳ���״̬
    else if (VIDEO_SEARCH == g_VideoFlag)       //�ɼ����� ��ʼ��ͼ����д���
    {
        ;
    }
    //ͬ��״̬
    else if (VIDEO_FINISH == g_VideoFlag)       //������� �ȴ�ͬ����һ��ͼ��
    {
            DISABLE_INTERRUPT_HREF();					//�������ж�
            ENABLE_INTERRUPT_VSYNC();					//���ж�ʹ��
            g_VideoFlag = VIDEO_WAIT;
    }

  
    enable_irq(INTERRUPT_PIT_4000uS_IRQ);			
    
}


void Search(void)
{
    int i, line, white_width = 0;
    //=======================ͼ��ʶ��=========================//
        //PS����ʱ���ǵ�ͼ���Ѿ���ȡ��� ���Ǵӵײ���ʼ������ʶ��ͼ��
        //ͼ��ʶ�����ݳ�ʼ��
        g_CenterNum = 0;
        g_LeftEdgeNum = 0;
        g_RightEdgeNum = 0;
        g_SearchFlag = 1;
        for (line = 0; line <= MAX_VIDEO_LINE - 1; line ++)
        {
            //���� ����������а׿��λ��
            g_SEnum = 0;  //���еİ׿����������
            g_BEnum = 0;  //���еĺڿ����������
            if (video_ori[line][MAX_VIDEO_POINT-1] > IMAGE_MIDDLE)  //������ֵΪ�� С����ֵΪ��
                g_Start[g_SEnum] = 0;
            for (i = MAX_VIDEO_POINT-1; i > 0; i --)
            {
                if (video_ori[line][i] > IMAGE_MIDDLE && video_ori[line][i-1] <= IMAGE_MIDDLE)
                {
                    g_End[g_SEnum] = MAX_VIDEO_POINT-i;
                    g_SEnum ++;
                    
                }
                else if (video_ori[line][i] <= IMAGE_MIDDLE && video_ori[line][i-1] > IMAGE_MIDDLE)
                {
                    g_Start[g_SEnum] = MAX_VIDEO_POINT-i-1;
                    g_BEnum ++;
                }
                
                else if (g_SEnum >= 10 || g_BEnum >=10)
                {
                    break;
                }
            }
            
            
            if (video_ori[line][0] > IMAGE_MIDDLE && g_SEnum < 10)
            {
                g_End[g_SEnum] = MAX_VIDEO_POINT-1;
                g_SEnum ++;
            }
            
            
            //�����������һ�е����ó������� ��Ϊ��������Ļ���
            if (g_SearchFlag)                     //����һ�д����־
            {
                int temp_mark = 0;
                if (g_SEnum == 0)
                {
                  continue;
                }
                white_width = g_End[0] - g_Start[0];
                for (i = 1; i < g_SEnum; i ++)   //ֱ�������İ׿�
                {
                    if (g_End[i] - g_Start[i] >= white_width)
                    {
                        white_width = g_End[i] - g_Start[i];
                        temp_mark = i;
                    }
                }
                if (white_width > WIDTH)
                {
                    g_SearchFlag = 0;
                    //��Ϊ��һ�����İ׿��������׿�
                    g_LeftEdge[g_LeftEdgeNum].x = line;
                    g_LeftEdge[g_LeftEdgeNum++].y = g_Start[temp_mark];
                    g_RightEdge[g_RightEdgeNum].x = line;
                    g_RightEdge[g_RightEdgeNum++].y = g_End[temp_mark];
                }
            }
            else
            {
                if (g_SEnum == 0)
                {
                    break;
                }
                for (i = 0,g_CoverNum = 0; i < g_SEnum; i ++)
                {
                    //���ǹ�ϵ����Ϊͼ����һ����ͨ���� ����������һ���Ѿ��ҵ��׿�����ͨ�İ׿�
                    g_Cover = MIN(g_End[i],g_RightEdge[g_RightEdgeNum-1].y) - MAX(g_Start[i],g_LeftEdge[g_LeftEdgeNum-1].y);
                    if (g_Cover > -1)
                    {
                        g_CoverIndex[g_CoverNum] = i;
                        g_CoverNum ++;
                    }
                }
                if (g_CoverNum == 1) //���ֻ��һ����ͨ�� ֱ��ȡ�������ͨ�׿�
                {
                    g_LeftEdge[g_LeftEdgeNum].x = line;
                    g_LeftEdge[g_LeftEdgeNum++].y = g_Start[g_CoverIndex[0]];
                    g_RightEdge[g_RightEdgeNum].x = line;
                    g_RightEdge[g_RightEdgeNum++].y = g_End[g_CoverIndex[0]];
                }
                else if (g_CoverNum == 0)  //û����ͨ�İ׿� ˵��ͼ���Ѿ��������
                {
                    break;                 //��������ͼ�����ѭ�� ���������Ĳ���
                }
                else //�ж���׿�   ��Ҫȡ��
                {
                    //ȡ�������������еİ׿�����ģ���Ѱ������һ�а׿������������һ���׿�
                    int temp_mark, temp_dis, temp_center, last_center;
                    temp_mark = 0;
                    temp_center = (g_Start[g_CoverIndex[0]] + g_End[g_CoverIndex[0]]) / 2;
                    last_center = (g_LeftEdge[g_LeftEdgeNum-1].y + g_RightEdge[g_RightEdgeNum-1].y) / 2;
                    temp_dis = ABS(last_center - temp_center);
                    for (i = 1; i < g_CoverNum; i ++)
                    {
                        temp_center = (g_Start[g_CoverIndex[i]] + g_End[g_CoverIndex[i]]) / 2;
                        if (ABS(temp_center-last_center) < temp_dis)
                        {
                            temp_dis = ABS(temp_center-last_center);
                            temp_mark = i;
                        }
                    }
                    g_LeftEdge[g_LeftEdgeNum].x = line;
                    g_LeftEdge[g_LeftEdgeNum++].y = g_Start[temp_mark];
                    g_RightEdge[g_RightEdgeNum].x = line;
                    g_RightEdge[g_RightEdgeNum++].y = g_End[temp_mark];
                }
            }
            
            
            //=======================����ж�========================//
//            if(g_SEnum >= 5 && g_BEnum >= 5)
//            {
//                times++;
//                if(times >= 20)
//                {
//                    g_PwmServo = 9245;
//                    Delay_T3_mS(1000);
//                    g_speed_final = 0; 
//                }
//                       //�����յ㣬����ٶȱ�Ϊ0
//            }
        }
        //=======================����ж�========================//

        
        //=======================ʮ�ִ���========================//
        
        
        //=======================������ȡ========================//
        /*
        *
        �˴���������ȡ�ǳ��򵥴ֲڣ���Ϊ֮ǰ�ڱ���������ʱ��û���������ߣ�
        ����ֱ�Ӱ�һ���׿��start��Ϊ����ߣ�end��Ϊ���ұ� ����������������
        �ߵ�����ȣ�������λ��ֱ�������ұ���ӳ�2����
        *
        */
                

//        //ԭ���Ĵ������£�
//        g_CenterNum = g_RightEdgeNum;
//        
//        for (i = 0; i < g_RightEdgeNum; i ++)
//        {
//            g_CenterPosition[i].x = g_RightEdge[i].x;
//            g_CenterPosition[i].y = (g_RightEdge[i].y + g_LeftEdge[i].y) / 2;         
//        }

       
        
        //�µ���������㷨
        
       g_CenterNum = g_RightEdgeNum;
       
       int Right_n = 0;                    //�ұ�Ե���� >= 150 �ĸ���
       int Left_n = 0;                     //���Ե���� <= 10 �ĸ���
       
       //int Right_black_n = 0;              //�����½�����ֺڿ�ʱ������
       //int Left_black_n = 0;               //�����½�����ֺڿ�ʱ������
       
       int L_black_x = 0, R_black_x = 0;           //���ҽ��������ֺڿ�������� (�漰����ֵ)           
       
       int Cross_n = 0;                    //��Ұ�г��ֹ�ͨ�׿������
       
       Move_Edg_L = ABS(g_LeftEdge[g_CenterNum-1].y - g_LeftEdge[0].y);
       Move_Edg_R = ABS(g_RightEdge[g_CenterNum-1].y - g_RightEdge[0].y);
       
       for(int s=0; s<g_CenterNum; s++)
       {
            if(g_LeftEdge[s].y <= 10 && g_RightEdge[s].y >= 150)
            {
                Cross_n++;
            }
       }
       
       for(int s=0; s<g_CenterNum; s++)
       {
            if(g_LeftEdge[s].y <= 10)         //��ת   ���кܶ��е�g_LeftEdge[] <= 10
            {
                Left_n++;
                if(Left_n == 1)
                {
                    L_black_x = s;
                }
            }
            else if(g_RightEdge[s].y >= 150)    //��ת   ���кܶ��е�g_RightEdge[] >= 150
            {
                Right_n++;
                if(Right_n == 1)
                {
                    R_black_x = s;
                }
            }
        }
       
        if( Left_n >= MIN(20, g_CenterNum) && Right_n <= 10 || (Move_Edg_R >= 100 && Move_Edg_L <= 50 ) )                  //��תϸ�¿���
        {
//            Str_YouWan = 0;
//            Str_ZhiDao = 0;
// 
//            for(i=0; i<Str_ZuoWan; i++)
//            {
//                if(g_speed_final >= 35)
//                {
//                   g_speed_final = 35; 
//                   break;
//                }
//                
//                g_speed_final = g_speed_final - 3;
//            }
//            
//            if(g_speed_final <= 35)
//              g_speed_final = 35;
//
//            if(Str_ZuoWan < 255)
//                Str_ZuoWan ++;
            
            if( L_black_x <= 3 )
            {
                for(i=0; i<g_CenterNum; i++)
                {
                    int Move_Edg = (g_RightEdge[i].y - g_LeftEdge[i].y) / 2;
                
                    g_CenterPosition[i].x = g_RightEdge[i].x;  
                    g_CenterPosition[i].y = (g_RightEdge[i].y - Move_Edg);
                }
            }
            else
            {
                for(i=0; i<g_CenterNum; i++)
                {
                    int Move_Edg = (g_RightEdge[i].y - g_LeftEdge[i].y)/2;
                
                    g_CenterPosition[i].x = g_RightEdge[i].x;   
                    g_CenterPosition[i].y = (g_LeftEdge[i].y + Move_Edg);
                }
            }
        }
        else if( Left_n<=10 && Right_n >= MIN(20,g_CenterNum) || (Move_Edg_L >= 100 && Move_Edg_R <= 50) )               //��תϸ�¿���
        {
//            
//            Str_ZuoWan = 0;
//            Str_ZhiDao = 0;
            
//            for(i=0; i<Str_YouWan; i++)
//            {
//
//                if(g_speed_final >= 35)
//                {
//                    g_speed_final = 35; 
//                    break;
//                }
//                
//                g_speed_final = g_speed_final - 3;
//            }
//            
//            if(g_speed_final <= 35)
//              g_speed_final = 35;
//
//            if(Str_YouWan < 255)
//                Str_YouWan ++;  
            
            if( R_black_x <= 3)
            {
                for (i=0; i<g_CenterNum; i++)
                {
                    int Move_Edg = (g_RightEdge[i].y - g_LeftEdge[i].y) / 2;
                    
                    g_CenterPosition[i].x = g_RightEdge[i].x;
                    g_CenterPosition[i].y = g_LeftEdge[i].y + Move_Edg;
                } 
            }
            else
            {
                for (i=0; i<g_CenterNum; i++)
                {
                    int Move_Edg = (g_RightEdge[i].y - g_LeftEdge[i].y)/2;
                    
                    g_CenterPosition[i].x = g_RightEdge[i].x;   
                    g_CenterPosition[i].y = g_RightEdge[i].y - Move_Edg;
                }
               
            }
        }
        else   //ֱ��
        {
          //       ֱ�м�����50
//            Str_ZuoWan = 0;
//            Str_YouWan = 0;
//          
//            for(i=0; i<Str_ZhiDao; i++)
//            {
//                if(g_speed_final >= 45)
//                {
//                  g_speed_final = 45;  
//                  break;
//                }
//                
//                g_speed_final = g_speed_final + 2;
//            }
//            
//            if(g_speed_final <= 45)
//              g_speed_final = 45;
//            
//            if(Str_ZhiDao < 255)
//                Str_ZhiDao ++;
              
           for (i = 0; i < g_RightEdgeNum; i ++)
           {
              g_CenterPosition[i].x = g_RightEdge[i].x;
              g_CenterPosition[i].y = (g_RightEdge[i].y + g_LeftEdge[i].y) / 2;           
           }       
       }
                  
       if(Cross_n >= 10 && Left_n > 20 && Right_n > 20)
       {
          for (i = 0; i < g_RightEdgeNum; i ++)
            
           {
              g_CenterPosition[i].x = g_RightEdge[i].x;
              g_CenterPosition[i].y = (g_RightEdge[i].y + g_LeftEdge[i].y) / 2;           
           }
       }
       
          
        //===================ת���Լ��������====================//
        //��Ȩƽ�����������
        g_DirectionControlWhole = 0;
        g_DirectionControlLine = 0;
        
        int tmpx, g_LowLine, g_HighLine;
        g_LowLine = MIN(g_CenterNum, 10);
        g_HighLine = MIN(g_CenterNum, 30);
        
        for (i = 0; i < MIN(g_CenterNum,50) ; i ++)
        {
            if (g_CenterPosition[i].y >= 0 && g_CenterPosition[i].y <= MAX_VIDEO_POINT)
            {
              
                //�Լ��Ľ����㷨
                if(g_CenterPosition[i].x < g_LowLine)
                  tmpx = ( (int) g_CenterPosition[i].x)*1;
                else if(g_CenterPosition[i].x > g_HighLine)
                  tmpx = ( (int) g_CenterPosition[i].x)*5;
                else
                  tmpx = ( (int) g_CenterPosition[i].x)*3;
                
                g_DirectionControlLine += tmpx;
                g_DirectionControlWhole += (int) g_CenterPosition[i].y * tmpx;
              
               // ԭ������
//                g_DirectionControlLine += (int)g_CenterPosition[i].x;
//                g_DirectionControlWhole += (int)g_CenterPosition[i].y * g_CenterPosition[i].x;  //ע�����ݲ�Ҫ���    c
            }
        }
        
        
        if (g_DirectionControlLine > 0)
        {
            g_DirectionControl = g_DirectionControlWhole / g_DirectionControlLine;
        }
        //��������޶�
        if (g_DirectionControl < 0)
        {
            g_DirectionControl = 0;
        }
        else if (g_DirectionControl > MAX_VIDEO_POINT)
        {
            g_DirectionControl = MAX_VIDEO_POINT;
        }
        if (ABS(g_DirectionControl - g_FormerDirectionControl) > 90)
        {
            g_DirectionControl = g_FormerDirectionControl;
        }
        //======================�������===========================//
        
        Car_Turn();
        
        //==================����λ������ͼ��=======================//
        if (g_SendPic)
        {
            // ͷͬ��
            uart_putvec("\xfe\x01\xfe\x01\xfe", 5);

            int16 k;
            // �����ܳ�
            k = 14 + MAX_VIDEO_LINE*MAX_VIDEO_POINT + 2 + MAX_POINT_NUM*8+4 + MAX_POINT_NUM*4+2 + 5;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ���
            k = g_SendIndex;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ����
            k = MAX_VIDEO_LINE;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ����
            k = MAX_VIDEO_POINT;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ͼ����ֵ
            k = IMAGE_MIDDLE;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ���ص���
            k = MAX_POINT_NUM;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // ���ĵ���
            k = MAX_POINT_NUM;
            uart_putchar(k % 256);uart_putchar(k / 256);

            // ͼ������
            uart_putvec(&video_ori[0][0], MAX_VIDEO_LINE*MAX_VIDEO_POINT);
            k = g_DirectionControl; // ��������
            uart_putchar(k % 256);uart_putchar(k / 256);

            // ���ص�
            k = g_LeftEdgeNum; // �������Ч����
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_LeftEdge[i].x, 2);// x
                uart_putvec((INT8U*)&g_LeftEdge[i].y, 2);// y
            }
            k = g_RightEdgeNum; // �ұ�����Ч����
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_RightEdge[i].x, 2);// x
                uart_putvec((INT8U*)&g_RightEdge[i].y, 2);// y
            }

            // ���ĵ�
            k = g_CenterNum; // ���ĵ���Ч����
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_CenterPosition[i].x, 2);// x
                uart_putvec((INT8U*)&g_CenterPosition[i].y, 2);// y
            }

            // ��������
            uart_putvec("\x15\x16\x17\x18\x19", 5);

            // βͬ��
            uart_putvec("\xfd\x02\xfd\x02\xfd", 5);

            g_SendIndex ++;
        }

        //======================״̬ת��=========================//
    
        g_VideoFlag = VIDEO_FINISH;
        g_Display++;                   //��¼����
        //�궨ʱ��
        if (g_Display % 5 == 0)
        {
          LED_1_REV();
        }
}



//��ʱʱ��
int32 g_time = 0;
/******************************************************************************/
/*
 *4ms�����ж�
 */
void pit_4000uS_isr(void)
{
    //clear the interrupt flag to avoid re-entrance
    CLR_FLAG_PIT4000uS();
    //��ʱ����
    TIME2_RESET();
    
    PULSE_COUNTER_GET(g_pulse);        //��ȡ���̷���ֵ    
    motorPID.vi_FeedBack = g_pulse;    //����ֵ����
    //�ٶȿ���
    Car_Run();
    
    g_time++;
    
}


/*
function name:  car_run
parameters:     none
author:         
date:           2017-9-10
description:    control car run forward
*/
void Car_Run(void)
{
//   if(g_time >800)
//   {
   
//     g_speed_final = 0;
    
//    }
    motorPID.vi_Ref = -g_speed_final;
    g_PwmMotor = v_PIDCalc(&motorPID);
    
    if(g_PwmMotor > 1000)
    {
        g_PwmMotor = 1000;
    }
    else if(g_PwmMotor < -1000)
    {
        g_PwmMotor = -1000;
    }

   // g_PwmMotor= -400;           //�������Ե����
    SET_PWM_MOTR(g_PwmMotor);

}


/*
function name:  car_turn
parameters:     none
author:         
date:           2017-9-10
description:    control car turn
*/
void Car_Turn(void)
{
    //��ֵת��Ϊƫ���������������ת��
    g_DirectionError = (MID_VIDEO_POINT - g_DirectionControl);
    
    g_PwmServo = (int)(PWM_SVO_MIDDLE + (g_DirectionError * turn_p) + (g_DirectionError - last_g_DirectionError) * turn_d);
    last_g_DirectionError = g_DirectionError;
      
   if (g_PwmServo <= PWM_SVO_MIN)
   {
       g_PwmServo = PWM_SVO_MIN;
   }
   else if (g_PwmServo >= PWM_SVO_MAX)
    {
       g_PwmServo = PWM_SVO_MAX;
    }
    //��������
 
    SET_PWM_SVO(g_PwmServo);
    
}     

