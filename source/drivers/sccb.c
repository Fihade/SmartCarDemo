#include "common.h"
#include "sccb.h"

#define MT9V022_REG_NUM 161
const unsigned int MT9V022_reg[MT9V022_REG_NUM][2];


unsigned char sccb_init()
{
	char _init_counter = 0;
	SCCB_SCL_PCR = (0 | PORT_PCR_MUX(1));
	SCCB_SDA_PCR = (0 | PORT_PCR_MUX(1));
	
	SET_BIT(SCCB_SCL_PDDR, SCCB_SCL_PIN);
	SET_BIT(SCCB_SDA_PDDR, SCCB_SDA_PIN);
	SCCB_SIC_L();
	SCCB_SID_L();
	
	while( _init_counter < 50)
	{
		_init_counter ++;
		if(1 == mt9v022_init())
			return 1;
	}
	return 0;
}



unsigned char mt9v022_init(void)
{
	unsigned int mt_init_counter = 0;
	if(0 ==  sccb_write(0x0c, 1))	{return 0;}
	Delay_T3_mS(10);
	if(0 ==  sccb_write(0x0c, 0))	{return 0;}
	Delay_T3_mS(15);		
	
	
	for(mt_init_counter = 0; mt_init_counter < MT9V022_REG_NUM; mt_init_counter ++)
	{
              Delay_T3_uS(2000);
		if(0 == sccb_write(MT9V022_reg[mt_init_counter][0], MT9V022_reg[mt_init_counter][1]))
		{ return 0; }
	}
        //if( 0==sccb_write(0x0d,0x0310 )){ return 0;}
	return 1;
}

/*
-----------------------------------------------
   功能: start命令,SCCB的起始信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //数据线高电平
    Delay_T3_uS(15);

    SCCB_SIC_H();	   //在时钟线高的时候数据线由高至低
    Delay_T3_uS(15);
 
    SCCB_SID_L();
    Delay_T3_uS(15);

    SCCB_SIC_L();	 //数据线恢复低电平，单操作函数必要
    Delay_T3_uS(15);
}
/*
-----------------------------------------------
   功能: stop命令,SCCB的停止信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID_L();
    Delay_T3_uS(15);
 
    SCCB_SIC_H();	
    Delay_T3_uS(15);
  

    SCCB_SID_H();	
    Delay_T3_uS(15);
}

/*
-----------------------------------------------
   功能: noAck,用于连续读取中的最后一个结束周期
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void noAck(void)
{
	SCCB_SID_H();	
	Delay_T3_uS(15);
	
	SCCB_SIC_H();	
	Delay_T3_uS(15);
	
	SCCB_SIC_L();	
	Delay_T3_uS(15);
	
	SCCB_SID_L();	
	Delay_T3_uS(15);
}
 
/*
-----------------------------------------------
   功能: Ack, 
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void Ack(void)
{
	SCCB_SID_L();	
	Delay_T3_uS(50);
	
	SCCB_SIC_H();	
	Delay_T3_uS(50);
	
	SCCB_SIC_L();	
	Delay_T3_uS(50);
	
	SCCB_SID_L();	
	Delay_T3_uS(50);
}
/*
-----------------------------------------------
   功能: 写入一个字节的数据到SCCB
   参数: 写入数据
 返回值: 发送成功返回1，发送失败返回0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) 				//循环8次发送数据
	{
		if((m_data<<j)&0x80)
		{
			SCCB_SID_H();	
		}
		else
		{
			SCCB_SID_L();	
		}
		Delay_T3_uS(14);
		SCCB_SIC_H();	
		Delay_T3_uS(15);
		SCCB_SIC_L();	
		Delay_T3_uS(1);

	}
	Delay_T3_uS(5);
	SCCB_SID_IN;					/*设置SDA为输入*/
	Delay_T3_uS(10);
	SCCB_SIC_H();	
	Delay_T3_uS(25);
	
	if(SCCB_SID_STATE){tem=0;}   	//SDA=1发送失败，返回0}
	else {tem=1;}   				//SDA=0发送成功，返回1
	
	SCCB_SIC_L();	
	Delay_T3_uS(10);	
    SCCB_SID_OUT;					/*设置SDA为输出*/

	return (tem);  
}

/*
-----------------------------------------------
   功能: 一个字节数据读取并且返回
   参数: 无
 返回值: 读取到的数据
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;/*设置SDA为输入*/
	Delay_T3_uS(10);
	for(j=8;j>0;j--) //循环8次接收数据
	{		     
		Delay_T3_uS(10);
		SCCB_SIC_H();
		Delay_T3_uS(10);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC_L();
		Delay_T3_uS(2);
	}
        SCCB_SID_OUT;/*设置SDA为输出*/	
	return(read);
}


////////////////////////////////////////////////////////////////////////////////

/*
-----------------------------------------------
   功能: 摄像头读寄存器
   参数: 
 返回值: 
-----------------------------------------------
*/
unsigned short sccb_read(unsigned char regID)
{
	unsigned short regDat = 0;
	startSCCB();
	if(0==SCCBwriteByte(MT9v022))//写地址
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	Delay_T3_uS(20);
  	if(0==SCCBwriteByte(regID))//积存器ID
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	stopSCCB();//发送SCCB 总线停止传输命令
	
	Delay_T3_uS(20);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(MT9v022+1))//读地址
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	Delay_T3_uS(20);
        regDat = SCCBreadByte();//返回读到的值
        Ack();//发送ACK命令
        regDat = (regDat<<8) + SCCBreadByte();//返回读到的值
        
  	noAck();//发送NACK命令
  	stopSCCB();//发送SCCB 总线停止传输命令
  	return regDat;//成功返回
}
/*
-----------------------------------------------
   功能: 摄像头写寄存器
   参数: 
 返回值: 
-----------------------------------------------
*/
unsigned char sccb_write(unsigned int regID, unsigned int regDat)
{
	startSCCB();//发送SCCB 总线开始传输命令
	if(0==SCCBwriteByte(MT9v022))//写地址
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	Delay_T3_uS(20);
  	if(0==SCCBwriteByte(regID))//积存器ID
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
	Delay_T3_uS(20);
  	if(0==SCCBwriteByte((unsigned char)(regDat>>8)))//写数据到积存器
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
        Delay_T3_uS(20);
  	if(0==SCCBwriteByte((unsigned char)regDat))//写数据到积存器
	{
		stopSCCB();//发送SCCB 总线停止传输命令
		return(0);//错误返回
	}
  	stopSCCB();//发送SCCB 总线停止传输命令
	
  	return(1);//成功返回
}

const unsigned int MT9V022_reg[MT9V022_REG_NUM][2]=
{
//[Reset]


   
   { 0x15,   0x7F32 },   	
   { 0x20,   0x01D1 },    	
   { 0x70,   0x0004 },   	//ROW_NOISE_CONTROL
   { 0x73,   0x02F7 },   	//DARK_COL_START
   { 0xC2,   0x040  },   	

   { 0x1C,   0x0003 },     	//DATA_COMPRESSION
   { 0x31,   0x001F },   	//V1_CONTROL
   { 0x32,   0x001A },   	//V2_CONTROL
   { 0x33,   0x0012 },   	//V3_CONTROL
   { 0xAF,   0x0000 },   	//AUTO_BLOCK_CONTROL
   { 0x2B,   0x0003 },   	
   { 0x10,   0x0040 },   

//;Default software settings

//LOAD=High Dynamic Settings
   { 0x0F,    0x0011 },	//PIXEL_OPERATION_MODE
   { 0x15,    0x7F32 },	
   { 0x1C,    0x0002 },	    //DATA_COMPRESSION

//[Reset Registers]
   { 1,    25 },	//COL_WINDOW_START_REG
   { 2,    4   },	//ROW_WINDOW_START_REG
   { 3,   480  },	//ROW_WINDOW_SIZE_REG  480
   { 4,   752  },	//COL_WINDOW_SIZE_REG  A 752
   { 5,    100  },	//HORZ_BLANK_REG       Q 45      A+Q = 846//470
   { 6,    31  },	//VERT_BLANK_REG  45
   { 7,   904  },	//CONTROL_MODE_REG  904
   { 8,   200  },	//SHUTTER_WIDTH_REG_1 443
   { 9,   150  },	//SHUTTER_WIDTH_REG_2 473
   { 10,   356 },	//SHUTTER_WIDTH_CONTROL
   { 11,   250 },	//INTEG_TIME_REG    480              //(752+ 100)*(480+45+480)/26.5M = 32.3ms
   { 12,     0 }, 	//RESET_REG
   { 13,   773 },	//READ_MODE_REG   768  772
   { 14,     0 }, 	//MONITOR_MODE_CONTROL
   { 15,    17 }, 	//PIXEL_OPERATION_MODE  17
   { 16,    45 }, 	
   { 17, 0x8042 },	
   { 18,    34 }, 	
   { 19,  3634 }, 	
   { 20,  3586 }, 	
   { 21,  3634 }, 	
   { 22, 10242 }, 	
   { 23, 15928 }, 	
   { 24, 15928 }, 	
   { 25, 10242 }, 	
   { 26,  1064 }, 	
   { 27,     0 }, 	//LED_OUT_CONTROL
   { 28,     2 }, 	//DATA_COMPRESSION
   { 29,     0 }, 	
   { 30,     0 }, 	
   { 31,     0 }, 	
   { 32,    17 }, 	
   { 33,    32 }, 	
   { 34,    32 }, 	
   { 35,    16 }, 	
   { 36,    16 }, 	
   { 37,    32 }, 	
   { 38,    16 }, 	
   { 39,    16 }, 	
   { 40,    16 }, 	
   { 41,    16 }, 	
   { 42,    32 }, 	
   { 43,     4 }, 	
   { 44,     4 }, 	
   { 45,     4 }, 	
   { 46,     7 }, 	
   { 47,     4 }, 	
   { 48,     3 }, 	
   { 49,    29 }, 	//V1_CONTROL
   { 50,    24 }, 	//V2_CONTROL
   { 51,    21 }, 	//V3_CONTROL
   { 52,     4 }, 	//V4_CONTROL
   { 53,    64 }, 	//GLOBAL_GAIN_REG
   { 54,    64 }, 	//MAXIMUM_GAIN_REG
   { 55,     0 }, 	
   { 56,     0 }, 	
   { 70,  8989 }, 	//DARK_AVG_THRESHOLDS
   { 71, 32896 }, 	//CALIB_CONTROL_REG      automatic black level correction
   { 76,     2 }, 	//STEP_SIZE_AVG_MODE
   { 96,     0 }, 	
   { 97,     0 }, 	
   { 98,     0 }, 	
   { 99,     0  },	
   { 100,     0 }, 	
   { 101,     0 }, 	
   { 102,     0 }, 	
   { 103,     0 }, 	
   { 108,     0 }, 	
   { 112,    52 }, 	//ROW_NOISE_CONTROL
   { 113,     0 }, 	
   { 114,   222 }, 	//NOISE_CONSTANT
   { 115,   759 }, 	//DARK_COL_START
   { 116,     0 }, 	//PIXCLK_CONTROL
   { 127,     0 }, 	//TEST_DATA
   { 128,   244 }, 	//TILE_X0_Y0
   { 129,   244 }, 	//TILE_X1_Y0
   { 130,   244 }, 	//TILE_X2_Y0
   { 131,   244 }, 	//TILE_X3_Y0
   { 132,   244 }, 	//TILE_X4_Y0
   { 133,   244 }, 	//TILE_X0_Y1
   { 134,   244 }, 	//TILE_X1_Y1
   { 135,   244 }, 	//TILE_X2_Y1
   { 136,   244 }, 	//TILE_X3_Y1
   { 137,   244 }, 	//TILE_X4_Y1
   { 138,   244 }, 	//TILE_X0_Y2
   { 139,   244 }, 	//TILE_X1_Y2
   { 140,   244 }, 	//TILE_X2_Y2
   { 141,   244 }, 	//TILE_X3_Y2
   { 142,   244 }, 	//TILE_X4_Y2
   { 143,   244 }, 	//TILE_X0_Y3
   { 144,   244 }, 	//TILE_X1_Y3
   { 145,   244 }, 	//TILE_X2_Y3
   { 146,   244 }, 	//TILE_X3_Y3
   { 147,   244 }, 	//TILE_X4_Y3
   { 148,   244 }, 	//TILE_X0_Y4
   { 149,   244 }, 	//TILE_X1_Y4
   { 150,   244 }, 	//TILE_X2_Y4
   { 151,   244 }, 	//TILE_X3_Y4
   { 152,   244 }, 	//TILE_X4_Y4
   { 153,     0 }, 	//X0_SLASH5
   { 154,   150 }, 	//X1_SLASH5
   { 155,   300 }, 	//X2_SLASH5
   { 156,   450 }, 	//X3_SLASH5
   { 157,   600 }, 	//X4_SLASH5
   { 158,   752 }, 	//X5_SLASH5
   { 159,     0 }, 	//Y0_SLASH5
   { 160,    96 }, 	//Y1_SLASH5
   { 161,   192 }, 	//Y2_SLASH5
   { 162,   288 }, 	//Y3_SLASH5
   { 163,   384 }, 	//Y4_SLASH5
   { 164,   480 }, 	//Y5_SLASH5
   { 165,    58 }, 	//DESIRED_BIN
   { 166,     2 }, 	//EXP_SKIP_FRM
   { 167,     0 }, 	
   { 168,     0  },	//EXP_LPF
   { 169,     2 }, 	//GAIN_SKIP_FRM_H
   { 170,     0  },	
   { 171,     2 }, 	//GAIN_LPF_H
   { 175,     3 },	//AUTO_BLOCK_CONTROL     3
   { 176, 44000 }, 	//PIXEL_COUNT
   { 177,     2 }, 	//LVDS_MASTER_CONTROL
   { 178,    16 }, 	//SHFT_CLK_CONTROL
   { 179,    16 }, 	//LVDS_DATA_CONTROL
   { 180,     0 }, 	//STREAM_LATENCY_SELECT
   { 181,     0 }, 	//LVDS_INTERNAL_SYNC
   { 182,     0 }, 	//USE_10BIT_PIXELS
   { 183,     0 }, 	//STEREO_ERROR_CONTROL
   { 189,   260 }, 	//MAX_EXPOSURE
   { 190,    20  },	
   { 191,    22  },	//INTERLACE_VBLANK
   { 192,    10 }, 	//IMAGE_CAPTURE_NUM
   { 194,  6208 }, 	
   {195,     0 }, 	//NTSC_FV_CONTROL
   {196, 17430 }, 	//NTSC_HBLANK
   {197, 17441 }, 	//NTSC_VBLANK
   {241,     0  },	//BYTEWISE_ADDR_REG
   {254, 48879 },  //REGISTER_LOCK_REG


   {0x74, 0x8  },
  
};
