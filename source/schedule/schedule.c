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
//============================== 图像数据 ================================//
Int16_point  g_CenterPosition[MAX_POINT_NUM];                           //中心线
Int16_point  g_LeftEdge[MAX_POINT_NUM], g_RightEdge[MAX_POINT_NUM];     //左右边沿
uint8 	video_ori[MAX_VIDEO_LINE][MAX_VIDEO_POINT] = {0};
int          g_CenterNum;                                               //中心点个数
int          g_LeftEdgeNum, g_RightEdgeNum;                             //左右边沿点个数
INT32U Temp = 0;                                                        //采集用缓冲变量
//============================== 图像处理用变量 ==========================//
uint16       g_Start[10], g_End[10], g_SEnum;                           //白块起点、白块终点、白块个数
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
int     turn_p = 0.0;
int     turn_d = 0.0;
//电机控制变量
int     g_pulse;                                                       //闭环PID控制的反馈值
int     g_speed_feedback;
int     g_PwmMotor;
//==============================数据传输===============================//
INT8U   g_SendPic = 1;                                                 //如果要用串口上位机看图像，则把该变量置一，建议看图像时把舵机和电机关闭
INT16U  g_SendIndex;
     



/******************************************************************************/
/******************************F U N C T I O N S  *****************************/
/******************************************************************************/
/*
* Schedule中变量初始化
*/
void ScheduleInit(void)//变量初始化
{
    Running_Init();               //运行参数初始化
    PID_Init();                   //PID初始化
                  
    //Choise();
    
    g_Line = 0;
    
    TIME2_interrupt_DIS(); 	 //电机中断服务程序关闭,PIT2  
    TIME2_RESET();
    
    ENABLE_INTERRUPT_HREF();//开启行中断
    ENABLE_INTERRUPT_VSYNC();//开启场中断
}

void Running_Init(void)//初始化
{
    if(JM_0_STATUS())//拨码开关
      LED_A_ON();
    
    if(JM_1_STATUS())//拨码开关
      LED_B_ON(); 
    
    if(JM_2_STATUS())//拨码开关
      LED_C_ON();   
    
    if(JM_3_STATUS())//拨码开关
      LED_D_ON();
  
    g_speed_p = 150;//电机PID，P
    g_speed_i = 90;//电机PID，I
    g_speed_d = 0;//电机PID，D
  
    turn_p = 10;//舵机PID。P
    turn_d = 2;//舵机PID。D
    
    g_speed_final = 30;//电机速度赋值
}

/*
 * 后台处理函数
 */
void Running(void)//主运行函数
{
    //============================采集及搜索中心==============================//  
    Search();

}

/*
 * vsync_isr场中断服务函数
 */
void vsync_isr(void)//场中断函数
{
    CLR_FLAG_VSYNC();  	        //清除场中断标志，开启行中断
    										
    g_Line = 0;
     
    ENABLE_INTERRUPT_HREF();   //开启行中断

    // 场序监控
    if(img_Cnt %5 ==0)
    {
      LED_0_REV();
    }
    img_Cnt++;
}

void href_isr(void)//行中断函数
{
    CLR_FLAG_HREF();	

    //=================================分行采集===============================//
    g_Line++;
        
    if (VIDEO_WAIT == g_VideoFlag)//等待状态
    {
        if (g_Line >= START_VIDEO_LINE)
        {
            g_VideoFlag = VIDEO_ACQUISITION;
            g_ImageLine = 0;
            g_LineToDiv = 3;
        }
    }
    //数据采集状态
    else if (VIDEO_ACQUISITION == g_VideoFlag)//请求状态
    {
        if (g_ImageLine >= 20 && g_LineToDiv == 3)   //分行采集方式，即上面20行是隔1行采一行 下面40行是隔2行采一行
            g_LineToDiv = 2;
        if(g_Line % g_LineToDiv == 0)
        {          
            int i;
		
            Delay_T3_uS(9);//整体图像右移（加大） 左移（减小）
		
            p = video_ori[g_ImageLine];
            
            for(i = 0; i < MAX_VIDEO_POINT; i++)   //覆盖式图像采集
            {
                Temp = (VIDEO_PORT)>>8;
                p[i] = (uint8)Temp;
                for (int m = 0; m < 3; m ++);
                 asm("nop");asm("nop");asm("nop");
		      asm("nop");asm("nop");
			asm("nop");asm("nop");asm("nop");
		      asm("nop");asm("nop");
			asm("nop");asm("nop");asm("nop");
		      asm("nop");asm("nop");//加大图像每一行点与点之间的间隔  增多使图像放大
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
    else if (VIDEO_SEARCH == g_VideoFlag)   //采集结束 开始对图像进行处理
    {
        ;
    }
    //同步状态
    else if (VIDEO_FINISH == g_VideoFlag)   //处理结束 等待同步下一场图像
    {
            DISABLE_INTERRUPT_HREF();					//禁用行中断
            ENABLE_INTERRUPT_VSYNC();					//场中断使能
            g_VideoFlag = VIDEO_WAIT;
    }

  
    enable_irq(INTERRUPT_PIT_4000uS_IRQ);			
    
}


void Search(void)//图像采集与处理，以及计算控制中心
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
            if (video_ori[line][MAX_VIDEO_POINT-1] > IMAGE_MIDDLE)  //大于阈值为白 小于阈值为黑 左边坐标原点
                g_Start[g_SEnum] = 0;
		
            for (i = MAX_VIDEO_POINT-1; i > 0; i --)//直接扫165个点，从第164（左）点开 第0（右）个数据点始扫到
            {
		  if (g_SEnum >= 10)//十个白块 退出
		  {
		    break;
		  }
		  else if (video_ori[line][i] > IMAGE_MIDDLE && video_ori[line][i-1] <= IMAGE_MIDDLE)//白块的终点
		  {
		    g_End[g_SEnum] = MAX_VIDEO_POINT-i;
		    g_SEnum ++;
		  }
		  else if (video_ori[line][i] <= IMAGE_MIDDLE && video_ori[line][i-1] > IMAGE_MIDDLE)//白块的起点
		  {
		    g_Start[g_SEnum] = MAX_VIDEO_POINT-i-1;
		  }   
		  
            }
            if (video_ori[line][0] > IMAGE_MIDDLE && g_SEnum < 10)//当最后一个点（右边）是白块时，为终点
            {
                g_End[g_SEnum] = MAX_VIDEO_POINT-1;
                g_SEnum ++;
            }
            //把最下面的那一行单独拿出来处理 做为后续处理的基础
		
		//在大FOR循环里面，先找到白块宽度大于60的那一行。
            if (g_SearchFlag)                    //最下一行处理标志  
            {
                int temp_mark = 0;
                if (g_SEnum == 0)
                {
                  continue;
                }
                white_width = g_End[0] - g_Start[0];
		    
                for (i = 1; i < g_SEnum; i ++)  //直接求最宽的白块
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
                if (g_SEnum == 0)//当这行没有白块，直接跳出大FOR循环
                {
                    break;
                }
		    //寻找这一行与上一行连通的白块
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
		    
		    //找到连通白块后，开始进行寻找最连通的白块
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
			  //找到那个最接近的白块，然后把左右边缘赋值
                    g_LeftEdge[g_LeftEdgeNum].x = line;
                    g_LeftEdge[g_LeftEdgeNum++].y = g_Start[g_CoverIndex[temp_mark]];
                    g_RightEdge[g_RightEdgeNum].x = line;
                    g_RightEdge[g_RightEdgeNum++].y = g_End[g_CoverIndex[temp_mark]];
                }
            }
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
        g_CenterNum = g_RightEdgeNum;
        for (i = 0; i < g_RightEdgeNum; i ++)
        {
            g_CenterPosition[i].x = g_RightEdge[i].x;
            g_CenterPosition[i].y = (g_RightEdge[i].y + g_LeftEdge[i].y) / 2;         
        }
    
        //===================转角以及电机控制====================//
        //加权平均法求方向控制
        g_DirectionControlWhole = 0;
        g_DirectionControlLine = 0;
        for (i = 0; i < g_CenterNum; i ++)
        {
            if (g_CenterPosition[i].y >= 0 && g_CenterPosition[i].y <= MAX_VIDEO_POINT)
            {
                g_DirectionControlLine += (int)g_CenterPosition[i].x;
                g_DirectionControlWhole += (int)g_CenterPosition[i].y * g_CenterPosition[i].x;  //注意数据不要溢出    
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
        g_Display ++;                   //记录场数
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
void pit_4000uS_isr(void)//4ms中断，用于控制电机，是个中断函数
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

void Car_Run(void)//电机控制函数，以及速度的赋值。
{

    motorPID.vi_Ref = g_speed_final;//将之前赋过值的变量给电机PID算法的参考值
    g_PwmMotor = v_PIDCalc(&motorPID);//根据电机PID算法，算出要输出的PWM值。
    
    if(g_PwmMotor > 1000)//保护
    {
        g_PwmMotor = 1000;
    }
    else if(g_PwmMotor < -1000)//保护
    {
        g_PwmMotor = -1000;
    }

   g_PwmMotor=200;
    SET_PWM_MOTR(g_PwmMotor);//最终输出给电机的PWM。

}

void Car_Turn(void)//舵机，转向控制函数
{
    //中值转换为偏离量，方便计算舵机转向
    g_DirectionError = (MID_VIDEO_POINT - g_DirectionControl);//MID_VIDEO_POINT是摄像头的中心变量，也就是车的位置变量。
                                                              //g_DirectionControl根据赛道图像算出来的中心，就是赛道的中心
                                                              //g_DirectionError赛道中心与图像中心的差值。
    
    g_PwmServo = (int)(PWM_SVO_MIDDLE + (g_DirectionError * turn_p) + (g_DirectionError - last_g_DirectionError) * turn_d);
    //是一个简单的舵机PID控制算法，用到了PD，所以我们得给turn_p和turn_d赋合适的值，让车能够完美的拐弯，根据速度不同要给不同的值。
    last_g_DirectionError = g_DirectionError;
    
    if (g_PwmServo <= PWM_SVO_MIN)//保护
    {
        g_PwmServo = PWM_SVO_MIN;
    }
    else if (g_PwmServo >= PWM_SVO_MAX)//保护
    {
        g_PwmServo = PWM_SVO_MAX;
    }
    //经过非门
    //SET_PWM_SVO(g_PwmServo);
    SET_PWM_SVO(g_PwmServo);
    
}     

