#ifndef __DMA_H_
#define __DMA_H_

#include "schedule.h"

/*--------------------------- M A C R O S ------------------------------------*/
#define 	APIC_EN  				1							//串口传送方式  0:队列传送	1:数组传送

#define 	IMG_H 					95//380							//IMAGE_LINE 
#define 	IMG_V 					330//246
#define         MAX_POINT			       IMG_V//220
#define		IMG_ORI_H				(IMG_H/2)
#define 	IMG_BINARY  			img_binary					//阈值

//#if(1)
////============================================================================//
//#define FRAME_SIZE  IMG_V 			// 队列单元个数，一个单元储存一行图像
//#define QUENE_SIZE  (IMG_H+10)   	// 24个队列
////============================================================================//
//#endif


#define DMA_CHx				(0)

#define DMA_TRG_PIN			(7)
#define DMA_TRG_DDR			GPIOC_PDDR
#define DMA_TRG_PCR			PORTC_PCR7

#define DMA_SOURCE_DAT(x)	PORTC_PCR(x)

#define  DMA_EN(DMA_CHn)        DMA_ERQ |= (DMA_ERQ_ERQ0_MASK<<(DMA_CHn))         //使能通道硬件DMA请求
#define  DMA_DIS(DMA_CHn)       DMA_ERQ &=~(DMA_ERQ_ERQ0_MASK<<(DMA_CHn))         //禁止通道硬件DMA请求



/*------------------------ D E F I N I T I O N S------------------------------*/
//extern uint8 	video_ori[IMG_ORI_H][IMG_V];
extern uint8 	video_ori[IMG_H+2][IMG_V];
extern uint8 	*g_VideoOri_ptr;
extern uint8 	img_flag;
extern uint8 	img_binary;
extern uint8 	g_PointInterval;
extern uint8 	base_img_binary;
extern uint8 	g_AllPic;								//发送灰度图标志

/*----------------------- D E C L A R A T I O N ------------------------------*/
void DMA_Init(void);
void DMA_CH0_isr(void);					//DMA行传输完毕中断
void vsync_isr(void);					//场中断
void href_isr(void);					//行中断

#endif

