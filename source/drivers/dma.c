#include "common.h"
#include "dma.h"

//==================================��������==================================//

uint8 	*g_VideoOri_ptr;
uint8 	img_binary;								//��ֵ����ֵ
uint8 	g_PointInterval;						//�����ֵ��
uint8 	base_img_binary;                                                       //������ֵ
//==================================��������==================================//

/******************************
 * sccbͼ�����
 * ���У�1~6
 * ��㣺112��114
******************************/

uint8 	img_flag = 0;							// 0: ���жϽ׶�	1:���жϽ׶�
INT8S 	img_Cnt = 0;							//���ж�ָʾ�Ƽ���
//INT16U 	img_row = 0;						//�ɼ�����������
//INT32U 	vsync_conter = 1;					//������
//INT32U 	href_conter = 0;					//�м���
//unsigned short href_cnt = 0;					// g_line
//unsigned short href_collect = 0;				// g_LineToDeal
INT8U g_AllPic = 0;					//���ڷ��ͻҶ�ͼ��־ 
uint8 g_AllPicSendingCnt = 0;				//�ȴ���������
uint8	g_AllPicSending = 0;			//0:δ��ͼ  1:�Ѿ��ɼ��ϰ볡  2:�Ѿ��ɼ��°볡�����Է���

//���д�����
//uint8 data_queue_1[QUENE_SIZE/2][FRAME_SIZE] = {0}; 	//�����СҪ<=64K��5K x 12 = 50K
//uint8 data_queue_2[QUENE_SIZE/2][FRAME_SIZE] = {1};     //�����������ݴ洢����


uint8 	video_ori[IMG_H+2][IMG_V] = {0};//������ �����������

//===================================��������=================================//
/*
 * DMA_CH0_isr
 */
void DMA_CH0_isr(void)
{
	CLR_FLAG_DMA_CH0();
	DMA_DIS(DMA_CHx);							//�д��������DMA_DISABLE
	//img_row++;									//DMA:�д���������ɼ�����+1
}

/*
 * vsync_isr
 */
void vsync_isr(void)
{
    CLR_FLAG_VSYNC(); 
   // sSPI_Data.time_4 = TIME1_GET_uS();
    //���ڷ��Ҷ�ͼ
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
    //vsync_conter ++;									//������

    g_line = 0;
    g_LineToUse = 0;

	if(!g_InterrupterFlag && sSPI_Data.display > 20)		//�ɼ������ڳ�ͼ�� && δ�����ٿ��ж�
	{
		TIME2_RESET();				// ����жϷ������ʹ��,PIT2 
		g_InterrupterFlag = 1;		// �ٿ��жϿ�����־
//		sSPI_Data.StartFlag = 1;
		vv_min = -100;
		vv_max = 1000;
	}
			
   DMA_DADDR(DMA_CHx) = (uint32)video_ori;				//���ô����ַ
    img_flag=0;

	// ������
    if(img_Cnt >=5)
    {
      LED_1_REV();
      img_Cnt = 0;
    }
    
    img_Cnt++;
    
    //TIME1_RESET();                              //������жϱ�־���������ж�
    ENABLE_INTERRUPT_HREF();     						//���ж�ʹ��
}

/*
 * href_isr
 */
void href_isr(void)
{
    CLR_FLAG_HREF();
	            
	//href_conter++;
 	// �ȴ���������ȥ

    //=================================���вɼ�===============================//
    g_line++;
    if(g_LineToUse < IMAGE_LINE
	   && g_line - START_VIDEO_LINE == g_line_select[g_LineToUse]
       && (img_flag!=1))
    {
         Delay_T3_uS(7); 
       	DMA_EN(DMA_CHx);							//���䵱ǰ������
        g_LineToUse++;								//׼��������һ�лҶ�ֵ
    }
    else if(g_line >= MAX_VIDEO_LINE)
	{
		img_flag = 1;					//�������жϱ�־
		DISABLE_INTERRUPT_HREF();					//�������ж�
		ENABLE_INTERRUPT_VSYNC();					//���ж�ʹ��
    }
    //========================================================================//
	
    if(g_line == 10)
    {
        sSPI_Data.display ++;
    }

  enable_irq(INTERRUPT_PIT_4000uS_IRQ);			// �����ʱ���жϽ���ȥ����
}

/*--------- DMA --------*/
/*
IO�ڴ��� PC18
Դ��ַ PD0~7
*/
void dma_port_init(void)
{
	DMA_TRG_PCR = 0 | PORT_PCR_MUX(1) | PORT_PCR_IRQC(1)  ;	// ����Դ PC18  ����  DMA�����ش������ڲ���������ʹ��
	CLR_BIT(DMA_TRG_DDR, DMA_TRG_PIN);		        // ����   PC18  Ϊ����

	GPIOC_PDDR &= 0xffff00ff ;				// PD0~7����Ϊ����

	for(int i = 8; i < 16; i++)
	{
		DMA_SOURCE_DAT(i) = 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK;
	}
}

void DMA_Init(void)
{
	/*����ʱ��*/
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  		       // ��DMAͨ����·������ʱ��
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;     		       // ��DMAģ��ʱ��


    /*����Դ��ַ��Ϣ*/
	DMA_SADDR(DMA_CHx) = (uint32)&GPIOC_PDIR+1;			// ����Դ��ַ GPIOD 0~7
	DMA_SOFF(DMA_CHx) = 0;								// ÿ�δ���Դ��ַ����


	/*����Ŀ�ĵ�ַ��Ϣ*/
	DMA_DADDR(DMA_CHx) = (uint32)video_ori;				// ����Ŀ�ĵ�ַ

	
	DMA_DOFF(DMA_CHx) = 1;						// ÿ��д��Ŀ�ĵ�ַ��1

	DMA_ATTR(DMA_CHx) = (0
					 | DMA_ATTR_SSIZE(0)			//8bit	// ����Դ���ݴ�����		8bit 16bit 32bit
					 | DMA_ATTR_SMOD(0)			// Դ��ַģ����ֹ  Source address modulo feature is disabled
					 | DMA_ATTR_DSIZE(0)				// ����Ŀ�ĵ�ַ�洢���		8bit 16bit 32bit
					 | DMA_ATTR_DMOD(0)					// Ŀ���ַģ����ֹ
					 );

	/*���ô���������*/
	DMA_CITER_ELINKNO(DMA_CHx) = DMA_CITER_ELINKNO_CITER(IMG_V);	// ��ǰ��ѭ���������ɼ�����
	DMA_BITER_ELINKNO(DMA_CHx) = DMA_BITER_ELINKNO_BITER(IMG_V);	// ��ʼ��ѭ���������ɼ�����

	DMA_CSR(DMA_CHx) = 0;
	DMA_CR &= ~DMA_CR_EMLM_MASK;                                    //��ֹ��ѭ��ӳ��
	DMA_NBYTES_MLNO(DMA_CHx) = DMA_NBYTES_MLNO_NBYTES(1);		// ÿ�ζ�ȡһ�ֽ�


	/* ����DMA���������Ĳ��� */
	DMA_SLAST(DMA_CHx) = 0;						// ��ѭ��������Դ��ַ����
	DMA_DLAST_SGA(DMA_CHx) = 0;					// ��ѭ��������Ŀ�ĵ�ַ��������ָ���������һ��
	DMA_CSR(DMA_CHx) = (0
						 | DMA_CSR_INTMAJOR_MASK		// ʹ��DMA0�ж� �����������ж�
						 | DMA_CSR_DREQ_MASK			// ��ѭ��������������ʱ���ر�DMA����
						);

	/* ����DMA����Դ */
	DMAMUX_CHCFG(DMA_CHx) = (0
							 | DMAMUX_CHCFG_ENBL_MASK	// ��DMA
							 | DMAMUX_CHCFG_SOURCE(51)	// DMAͨ��0  ����Դ PORTC PC18
							 );


	dma_port_init();							// DMA�˿ڳ�ʼ��

	enable_irq(INTERRUPT_DMA_CH0_IRQ);
	//	DMA_TCD0_CSR&=~DMA_CSR_DREQ_MASK&~DMA_CSR_INTMAJOR_MASK
	//	&~DMA_CSR_ESG_MASK&~DMA_CSR_INTHALF_MASK;
}
