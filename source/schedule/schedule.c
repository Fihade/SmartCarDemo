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
//=============================== 数据采集 ================================//
int          g_Line;                                                    //摄像头行数
int          g_ImageLine;                                               //图像数据行数
Video_state  g_VideoFlag = VIDEO_WAIT;                                  //处理图像状态标志
int          g_Display;                                                 //场数 系统计时
uint8        *p = NULL;                                                 //图像采集用指针
int          g_LineToDiv;                                               //隔行采集用除数
int          img_Cnt;
//=============================== 图像数据 ================================//
Int16_point  g_CenterPosition[MAX_POINT_NUM];                           //中心线
Int16_point  g_LeftEdge[MAX_POINT_NUM], g_RightEdge[MAX_POINT_NUM];     //左右边沿
uint8 	     video_ori[MAX_VIDEO_LINE][MAX_VIDEO_POINT] = {0};
int          g_CenterNum;                                               //中心点个数
int          g_LeftEdgeNum, g_RightEdgeNum;                             //左右边沿点个数
INT32U Temp = 0;                                                        //采集用缓冲变量
//============================== 图像处理用变量 ==========================//
uint16       g_Start[10], g_End[10], g_SEnum, g_BEnum;                           //白块起点、白块终点、白块个数、 黑块个数
uint8        g_SearchFlag;
int          g_Cover;
uint8        g_CoverIndex[10],g_CoverNum;

//==============================控制用变量==============================//
//舵机控制变量
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
//电机控制变量
int     g_pulse;                                                       //闭环PID控制的反馈值
int     g_speed_feedback;
int     g_PwmMotor;
//==============================数据传输===============================//
INT8U   g_SendPic = 0;                                                //如果要用串口上位机看图像，则把该变量置一，建议看图像时把舵机和电机关闭！！！！！！！！！！！！！
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
* Schedule中变量初始化
*/
void ScheduleInit(void)
{
    Running_Init();               //运行参数初始化
    PID_Init();                   //PID初始化
                  
    //Choise();                   //键盘函数，暂时不用
    
    g_Line = 0;
    
    TIME2_interrupt_DIS(); 	 //电机中断服务程序关闭,PIT2  
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
    //拨码开关

  g_speed_p = 150;                //电机pid 
  g_speed_i = 70;
  g_speed_d = 1;
  
  
  if(JM_0_STATUS())//1 往下
  {
    LED_A_ON();    
    turn_p = 7;                  //舵机pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //舵机pid d       1     1    1.2     1.2         1        1       1    
    g_speed_final = 40;          //电机速度赋值
  }
  
  else if(JM_1_STATUS())
  {
    LED_B_ON(); 
    turn_p = 7.4;                  //舵机pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.5;                    //舵机pid d       1     1    1.2     1.2         1        1       1    
    g_speed_final = 40;          //电机速度赋值
  }
  
  else if(JM_2_STATUS())
  {
    LED_C_ON();   
    turn_p = 7.4;                  //舵机pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //舵机pid d       1     1    1.2     1.2         1        1       1   
    g_speed_final = 40;          //电机速度赋值
  }
  
  else  if(JM_3_STATUS())
  {
    //
    LED_D_ON();   
    turn_p = 7.6;                  //舵机pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //舵机pid d       1     1    1.2     1.2         1        1       1  
    g_speed_final = 40;          //电机速度赋值
  }
  else
  {
    turn_p = 7.8;                  //舵机pid p      11     8     10      9        9     8.5     8.6
    turn_d = 0.7;                    //舵机pid d       1     1    1.2     1.2         1        1       1  
    g_speed_final = 40;          //电机速度赋值
  }
  
}

/******************************************************************************/
/*
* 后台处理函数
*/
void Running(void)
{
  //============================采集及搜索中心==============================//  
  Search();
  
}


/*
* vsync_isr场中断服务函数，
*/
void vsync_isr(void)
{
  CLR_FLAG_VSYNC();  	        //清除场中断标志，开启行中断
  
  g_Line = 0;
  
  ENABLE_INTERRUPT_HREF();   //开启行中断，行中断集满60行之后换场
  
  // 场序监控
  if(img_Cnt %5 ==0)
  {
    LED_0_REV();
  }
  img_Cnt++;
}

/*
* href_isr行中断服务函数
*/
void href_isr(void)
{
  CLR_FLAG_HREF();	
  
  //=================================分行采集===============================// 
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
  //数据采集状态
  else if (VIDEO_ACQUISITION == g_VideoFlag)
  {
    if (g_ImageLine >= 20 && g_LineToDiv == 3)   //分行采集方式，即上面20行是隔1行采一行 下面40行是隔2行采一行
      g_LineToDiv = 2;
    if(g_Line % g_LineToDiv == 0)
    {          
      int i;
      Delay_T3_uS(6.5);
      p = video_ori[g_ImageLine];
      
      for(i = 0; i < MAX_VIDEO_POINT; i++)   //覆盖式图像采集
      {
        Temp = (VIDEO_PORT)>>8;
        p[i] = (uint8)Temp;
        for (int m = 0; m < 3; m ++);
        asm("nop");asm("nop");asm("nop");
        //这里也可以写作“g_VideoImageData[g_VideoImageLine][i]=(VIDEO_PORT);”大家自己看看这两种写法的区别
      }
      
            g_ImageLine ++;
        }
        if(g_ImageLine >= MAX_VIDEO_LINE)
        {
            g_VideoFlag = VIDEO_SEARCH;         
        }
    }
    //图像处理和控制状态
    else if (VIDEO_SEARCH == g_VideoFlag)       //采集结束 开始对图像进行处理
    {
        ;
    }
    //同步状态
    else if (VIDEO_FINISH == g_VideoFlag)       //处理结束 等待同步下一场图像
    {
            DISABLE_INTERRUPT_HREF();					//禁用行中断
            ENABLE_INTERRUPT_VSYNC();					//场中断使能
            g_VideoFlag = VIDEO_WAIT;
    }

  
    enable_irq(INTERRUPT_PIT_4000uS_IRQ);			
    
}


void Search(void)
{
    int i, line, white_width = 0;
    //=======================图像识别=========================//
        //PS：此时我们的图像已经提取完毕 我们从底部开始，向上识别图像
        //图像识别数据初始化
        g_CenterNum = 0;
        g_LeftEdgeNum = 0;
        g_RightEdgeNum = 0;
        g_SearchFlag = 1;
        for (line = 0; line <= MAX_VIDEO_LINE - 1; line ++)
        {
            //首先 求出本行所有白块的位置
            g_SEnum = 0;  //本行的白块计数器清零
            g_BEnum = 0;  //本行的黑块计数器清零
            if (video_ori[line][MAX_VIDEO_POINT-1] > IMAGE_MIDDLE)  //大于阈值为白 小于阈值为黑
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
            
            
            //把最下面的那一行单独拿出来处理 做为后续处理的基础
            if (g_SearchFlag)                     //最下一行处理标志
            {
                int temp_mark = 0;
                if (g_SEnum == 0)
                {
                  continue;
                }
                white_width = g_End[0] - g_Start[0];
                for (i = 1; i < g_SEnum; i ++)   //直接求最宽的白块
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
                    //认为第一行最宽的白块便是所需白块
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
                    //覆盖关系，因为图像是一个联通区域 于是找与上一行已经找到白块相联通的白块
                    g_Cover = MIN(g_End[i],g_RightEdge[g_RightEdgeNum-1].y) - MAX(g_Start[i],g_LeftEdge[g_LeftEdgeNum-1].y);
                    if (g_Cover > -1)
                    {
                        g_CoverIndex[g_CoverNum] = i;
                        g_CoverNum ++;
                    }
                }
                if (g_CoverNum == 1) //如果只有一个联通的 直接取出这个联通白块
                {
                    g_LeftEdge[g_LeftEdgeNum].x = line;
                    g_LeftEdge[g_LeftEdgeNum++].y = g_Start[g_CoverIndex[0]];
                    g_RightEdge[g_RightEdgeNum].x = line;
                    g_RightEdge[g_RightEdgeNum++].y = g_End[g_CoverIndex[0]];
                }
                else if (g_CoverNum == 0)  //没有联通的白块 说明图像已经处理完毕
                {
                    break;                 //跳出整个图像处理的循环 进入求中心部分
                }
                else //有多个白块   需要取舍
                {
                    //取舍规则：先求出本行的白块的中心，再寻找与上一行白块中心最相近的一个白块
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
            
            
            //=======================起点判断========================//
//            if(g_SEnum >= 5 && g_BEnum >= 5)
//            {
//                times++;
//                if(times >= 20)
//                {
//                    g_PwmServo = 9245;
//                    Delay_T3_mS(1000);
//                    g_speed_final = 0; 
//                }
//                       //到达终点，电机速度变为0
//            }
        }
        //=======================起点判断========================//

        
        //=======================十字处理========================//
        
        
        //=======================中心求取========================//
        /*
        *
        此处的中心求取非常简单粗糙，因为之前在边沿搜索的时候并没有区分两边，
        而是直接把一个白块的start认为是左边，end认为是右边 故中心数将与左右
        边的数相等，而中心位置直接由左右边相加除2所得
        *
        */
                

//        //原来的代码如下：
//        g_CenterNum = g_RightEdgeNum;
//        
//        for (i = 0; i < g_RightEdgeNum; i ++)
//        {
//            g_CenterPosition[i].x = g_RightEdge[i].x;
//            g_CenterPosition[i].y = (g_RightEdge[i].y + g_LeftEdge[i].y) / 2;         
//        }

       
        
        //新的弯道矫正算法
        
       g_CenterNum = g_RightEdgeNum;
       
       int Right_n = 0;                    //右边缘坐标 >= 150 的个数
       int Left_n = 0;                     //左边缘坐标 <= 10 的个数
       
       //int Right_black_n = 0;              //当右下角落出现黑块时的行数
       //int Left_black_n = 0;               //当左下角落出现黑块时的行数
       
       int L_black_x = 0, R_black_x = 0;           //左右角落最后出现黑块的纵坐标 (涉及到阈值)           
       
       int Cross_n = 0;                    //视野中出现贯通白块的行数
       
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
            if(g_LeftEdge[s].y <= 10)         //左转   即有很多行的g_LeftEdge[] <= 10
            {
                Left_n++;
                if(Left_n == 1)
                {
                    L_black_x = s;
                }
            }
            else if(g_RightEdge[s].y >= 150)    //右转   即有很多行的g_RightEdge[] >= 150
            {
                Right_n++;
                if(Right_n == 1)
                {
                    R_black_x = s;
                }
            }
        }
       
        if( Left_n >= MIN(20, g_CenterNum) && Right_n <= 10 || (Move_Edg_R >= 100 && Move_Edg_L <= 50 ) )                  //左转细致考虑
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
        else if( Left_n<=10 && Right_n >= MIN(20,g_CenterNum) || (Move_Edg_L >= 100 && Move_Edg_R <= 50) )               //右转细致考虑
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
        else   //直行
        {
          //       直行加速至50
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
       
          
        //===================转角以及电机控制====================//
        //加权平均法求方向控制
        g_DirectionControlWhole = 0;
        g_DirectionControlLine = 0;
        
        int tmpx, g_LowLine, g_HighLine;
        g_LowLine = MIN(g_CenterNum, 10);
        g_HighLine = MIN(g_CenterNum, 30);
        
        for (i = 0; i < MIN(g_CenterNum,50) ; i ++)
        {
            if (g_CenterPosition[i].y >= 0 && g_CenterPosition[i].y <= MAX_VIDEO_POINT)
            {
              
                //自己改进的算法
                if(g_CenterPosition[i].x < g_LowLine)
                  tmpx = ( (int) g_CenterPosition[i].x)*1;
                else if(g_CenterPosition[i].x > g_HighLine)
                  tmpx = ( (int) g_CenterPosition[i].x)*5;
                else
                  tmpx = ( (int) g_CenterPosition[i].x)*3;
                
                g_DirectionControlLine += tmpx;
                g_DirectionControlWhole += (int) g_CenterPosition[i].y * tmpx;
              
               // 原来代码
//                g_DirectionControlLine += (int)g_CenterPosition[i].x;
//                g_DirectionControlWhole += (int)g_CenterPosition[i].y * g_CenterPosition[i].x;  //注意数据不要溢出    c
            }
        }
        
        
        if (g_DirectionControlLine > 0)
        {
            g_DirectionControl = g_DirectionControlWhole / g_DirectionControlLine;
        }
        //方向控制限定
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
        //======================舵机控制===========================//
        
        Car_Turn();
        
        //==================向上位机发送图像=======================//
        if (g_SendPic)
        {
            // 头同步
            uart_putvec("\xfe\x01\xfe\x01\xfe", 5);

            int16 k;
            // 数据总长
            k = 14 + MAX_VIDEO_LINE*MAX_VIDEO_POINT + 2 + MAX_POINT_NUM*8+4 + MAX_POINT_NUM*4+2 + 5;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 序号
            k = g_SendIndex;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 行数
            k = MAX_VIDEO_LINE;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 列数
            k = MAX_VIDEO_POINT;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 图像阈值
            k = IMAGE_MIDDLE;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 边沿点数
            k = MAX_POINT_NUM;
            uart_putchar(k % 256);uart_putchar(k / 256);
            // 中心点数
            k = MAX_POINT_NUM;
            uart_putchar(k % 256);uart_putchar(k / 256);

            // 图像数据
            uart_putvec(&video_ori[0][0], MAX_VIDEO_LINE*MAX_VIDEO_POINT);
            k = g_DirectionControl; // 控制中心
            uart_putchar(k % 256);uart_putchar(k / 256);

            // 边沿点
            k = g_LeftEdgeNum; // 左边沿有效点数
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_LeftEdge[i].x, 2);// x
                uart_putvec((INT8U*)&g_LeftEdge[i].y, 2);// y
            }
            k = g_RightEdgeNum; // 右边沿有效点数
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_RightEdge[i].x, 2);// x
                uart_putvec((INT8U*)&g_RightEdge[i].y, 2);// y
            }

            // 中心点
            k = g_CenterNum; // 中心点有效个数
            uart_putchar(k % 256);uart_putchar(k / 256);
            for(i=0; i<MAX_POINT_NUM; i++)
            {
                uart_putvec((INT8U*)&g_CenterPosition[i].x, 2);// x
                uart_putvec((INT8U*)&g_CenterPosition[i].y, 2);// y
            }

            // 其他数据
            uart_putvec("\x15\x16\x17\x18\x19", 5);

            // 尾同步
            uart_putvec("\xfd\x02\xfd\x02\xfd", 5);

            g_SendIndex ++;
        }

        //======================状态转换=========================//
    
        g_VideoFlag = VIDEO_FINISH;
        g_Display++;                   //记录场数
        //标定时序
        if (g_Display % 5 == 0)
        {
          LED_1_REV();
        }
}



//计时时间
int32 g_time = 0;
/******************************************************************************/
/*
 *4ms控制中断
 */
void pit_4000uS_isr(void)
{
    //clear the interrupt flag to avoid re-entrance
    CLR_FLAG_PIT4000uS();
    //计时重置
    TIME2_RESET();
    
    PULSE_COUNTER_GET(g_pulse);        //读取码盘反馈值    
    motorPID.vi_FeedBack = g_pulse;    //反馈值采样
    //速度控制
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

   // g_PwmMotor= -400;           //开环测试电机用
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
    //中值转换为偏离量，方便计算舵机转向
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
    //经过非门
 
    SET_PWM_SVO(g_PwmServo);
    
}     

