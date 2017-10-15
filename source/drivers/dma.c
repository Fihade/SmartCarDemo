#include "common.h"
#include "dma.h"

//==================================变量定义==================================//

uint8 	*g_VideoOri_ptr;
uint8 	img_binary;								//二值化阈值
uint8 	g_PointInterval;						//跳点二值化
uint8 	base_img_binary;                                                       //基础阈值
//==================================变量定义==================================//

/******************************
 * sccb图像调整
 * 行列：1~6
 * 噪点：112，114
******************************/

uint8 	img_flag = 0;							// 0: 行中断阶段	1:场中断阶段
INT8S 	img_Cnt = 0;							//场中断指示灯计数
//INT16U 	img_row = 0;						//采集的行数计数
//INT32U 	vsync_conter = 1;					//场计数
//INT32U 	href_conter = 0;					//行计数
//unsigned short href_cnt = 0;					// g_line
//unsigned short href_collect = 0;				// g_LineToDeal
INT8U g_AllPic = 0;					//串口发送灰度图标志 
uint8 g_AllPicSendingCnt = 0;				//等待换场计数
uint8	g_AllPicSending = 0;			//0:未采图  1:已经采集上半场  2:已经采集下半场，可以发送

//队列传数据
//uint8 data_queue_1[QUENE_SIZE/2][FRAME_SIZE] = {0}; 	//数组大小要<=64K，5K x 12 = 50K
//uint8 data_queue_2[QUENE_SIZE/2][FRAME_SIZE] = {1};     //故用两个数据存储队列


uint8 	video_ori[IMG_H+2][IMG_V] = {0};//加两行 解决卡死问题

//===================================函数定义=================================//
/*
 * DMA_CH0_isr
 */
void DMA_CH0_isr(void)
{
	CLR_FLAG_DMA_CH0();
	DMA_DIS(DMA_CHx);							//行传输结束，DMA_DISABLE
	//img_row++;									//DMA:行传输结束，采集行数+1
}

/*
 * vsync_isr
 */
void vsync_isr(void)
{
    CLR_FLAG_VSYNC(); 
   // sSPI_Data.time_4 = TIME1_GET_uS();
    //串口发灰度图
#if(1==PIC_EN)
	if(g_AllPic == 1)
	{
		if(g_AllPicSendingCnt < 3)
		{
			g_AllPicSendingCnt ++;
		}
		if(img_flag == 1 && g_AllPicSendingCnt >= 3)
		{
			DISABLE_INTERRUPT_VSYNC();
			DISABLE_INTERRUPT_HREF();
			for(int i = 0; i < IMG_H; i++)
			{
				uint8 *p = &video_ori[i][0];
				for(int j = 0; j < IMG_V; j++)
				{
					if(p[j] == 0xff)
					{
						p[j] = 0xfe;
					}
					uart_putchar(p[j]);
				}
			}
			uart_putchar(0xff);
			ENABLE_INTERRUPT_VSYNC();
			
			g_AllPicSendingCnt = 0;
		}
	}
#endif
	

    //img_row = 0;
    //href_conter = 0;
    //vsync_conter ++;									//场计数

    g_line = 0;
    g_LineToUse = 0;

	if(!g_InterrupterFlag && sSPI_Data.display > 20)		//采集到多于场图像 && 未开启速控中断
	{
		TIME2_RESET();				// 电机中断服务程序使能,PIT2 
		g_InterrupterFlag = 1;		// 速控中断开启标志
//		sSPI_Data.StartFlag = 1;
		vv_min = -100;
		vv_max = 1000;
	}
			
   DMA_DADDR(DMA_CHx) = (uint32)video_ori;				//设置传输地址
    img_flag=0;

	// 场序监控
    if(img_Cnt >=5)
    {
      LED_1_REV();
      img_Cnt = 0;
    }
    
    img_Cnt++;
    
    //TIME1_RESET();                              //清除场中断标志，开启行中断
    ENABLE_INTERRUPT_HREF();     						//行中断使能
}

/*
 * href_isr
 */
void href_isr(void)
{
    CLR_FLAG_HREF();
	            
	//href_conter++;
 	// 等待消隐区过去

    //=================================分行采集===============================//
    g_line++;
    if(g_LineToUse < IMAGE_LINE
	   && g_line - START_VIDEO_LINE == g_line_select[g_LineToUse]
       && (img_flag!=1))
    {
         Delay_T3_uS(7); 
       	DMA_EN(DMA_CHx);							//传输当前行数据
        g_LineToUse++;								//准备接收下一行灰度值
    }
    else if(g_line >= MAX_VIDEO_LINE)
	{
		img_flag = 1;					//开启场中断标志
		DISABLE_INTERRUPT_HREF();					//禁用行中断
		ENABLE_INTERRUPT_VSYNC();					//场中断使能
    }
    //========================================================================//
	
    if(g_line == 10)
    {
        sSPI_Data.display ++;
    }

  enable_irq(INTERRUPT_PIT_4000uS_IRQ);			// 解决计时器中断进不去问题
}

/*--------- DMA --------*/
/*
IO口触发 PC18
源地址 PD0~7
*/
void dma_port_init(void)
{
	DMA_TRG_PCR = 0 | PORT_PCR_MUX(1) | PORT_PCR_IRQC(1)  ;	// 触发源 PC18  配置  DMA上升沿触发，内部下拉电阻使能
	CLR_BIT(DMA_TRG_DDR, DMA_TRG_PIN);		        // 配置   PC18  为输入

	GPIOC_PDDR &= 0xffff00ff ;				// PD0~7配置为输入

	for(int i = 8; i < 16; i++)
	{
		DMA_SOURCE_DAT(i) = 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK;
	}
}

void DMA_Init(void)
{
	/*开启时钟*/
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  		       // 打开DMA通道多路复用器时钟
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;     		       // 打开DMA模块时钟


    /*设置源地址信息*/
	DMA_SADDR(DMA_CHx) = (uint32)&GPIOC_PDIR+1;			// 设置源地址 GPIOD 0~7
	DMA_SOFF(DMA_CHx) = 0;								// 每次传送源地址不变


	/*设置目的地址信息*/
	DMA_DADDR(DMA_CHx) = (uint32)video_ori;				// 设置目的地址

	
	DMA_DOFF(DMA_CHx) = 1;						// 每次写入目的地址加1

	DMA_ATTR(DMA_CHx) = (0
					 | DMA_ATTR_SSIZE(0)			//8bit	// 设置源数据传输宽度		8bit 16bit 32bit
					 | DMA_ATTR_SMOD(0)			// 源地址模数禁止  Source address modulo feature is disabled
					 | DMA_ATTR_DSIZE(0)				// 设置目的地址存储宽度		8bit 16bit 32bit
					 | DMA_ATTR_DMOD(0)					// 目标地址模数禁止
					 );

	/*设置传输数据量*/
	DMA_CITER_ELINKNO(DMA_CHx) = DMA_CITER_ELINKNO_CITER(IMG_V);	// 当前主循环次数，采集点数
	DMA_BITER_ELINKNO(DMA_CHx) = DMA_BITER_ELINKNO_BITER(IMG_V);	// 起始主循环次数，采集点数

	DMA_CSR(DMA_CHx) = 0;
	DMA_CR &= ~DMA_CR_EMLM_MASK;                                    //禁止副循环映射
	DMA_NBYTES_MLNO(DMA_CHx) = DMA_NBYTES_MLNO_NBYTES(1);		// 每次读取一字节


	/* 配置DMA传输结束后的操作 */
	DMA_SLAST(DMA_CHx) = 0;						// 主循环结束后源地址不变
	DMA_DLAST_SGA(DMA_CHx) = 0;					// 主循环结束后目的地址不调整，指向数组的下一行
	DMA_CSR(DMA_CHx) = (0
						 | DMA_CSR_INTMAJOR_MASK		// 使能DMA0中断 计数减到零中断
						 | DMA_CSR_DREQ_MASK			// 主循环计数器等于零时，关闭DMA请求
						);

	/* 配置DMA触发源 */
	DMAMUX_CHCFG(DMA_CHx) = (0
							 | DMAMUX_CHCFG_ENBL_MASK	// 打开DMA
							 | DMAMUX_CHCFG_SOURCE(51)	// DMA通道0  触发源 PORTC PC18
							 );


	dma_port_init();							// DMA端口初始化

	enable_irq(INTERRUPT_DMA_CH0_IRQ);
	//	DMA_TCD0_CSR&=~DMA_CSR_DREQ_MASK&~DMA_CSR_INTMAJOR_MASK
	//	&~DMA_CSR_ESG_MASK&~DMA_CSR_INTHALF_MASK;
}
