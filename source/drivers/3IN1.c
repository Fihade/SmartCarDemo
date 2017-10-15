#if(1)
#include "common.h"
#include "3IN1.h"
#include "i2chard.h"


unsigned char OLED_RAM_add[128][8] = {0};
uint16 EEPROM_middle_0608;
uint16 EEPROM_middle_0816;
uint8 EEPROM_high_0816=0x00;
uint8 EEPROM_low_0816=0x00;
uint8 EEPROM_high_0608=0x05;
uint8 EEPROM_low_0608=0xf0;

uint8 temp_test[6]={0};
/****************OLED显示屏初始化**********************/
void Init_OLED()
{
  i2c_write_register_add(device_add0,reg_add0,0xae);//--turn off oled panel
  i2c_write_register_add(device_add0,reg_add0,0x00);//---set low column address
  i2c_write_register_add(device_add0,reg_add0,0x10);//---set high column address
  i2c_write_register_add(device_add0,reg_add0,0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  i2c_write_register_add(device_add0,reg_add0,0x81);//--set contrast control register
  i2c_write_register_add(device_add0,reg_add0,0xcf); // Set SEG Output Current Brightness
  i2c_write_register_add(device_add0,reg_add0,0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  i2c_write_register_add(device_add0,reg_add0,0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  i2c_write_register_add(device_add0,reg_add0,0xa6);//--set normal display
  i2c_write_register_add(device_add0,reg_add0,0xa8);//--set multiplex ratio(1 to 64)
  i2c_write_register_add(device_add0,reg_add0,0x3f);//--1/64 duty
  i2c_write_register_add(device_add0,reg_add0,0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  i2c_write_register_add(device_add0,reg_add0,0x00);//-not offset
  i2c_write_register_add(device_add0,reg_add0,0xd5);//--set display clock divide ratio/oscillator frequency
  i2c_write_register_add(device_add0,reg_add0,0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
  i2c_write_register_add(device_add0,reg_add0,0xd9);//--set pre-charge period
  i2c_write_register_add(device_add0,reg_add0,0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  i2c_write_register_add(device_add0,reg_add0,0xda);//--set com pins hardware configuration
  i2c_write_register_add(device_add0,reg_add0,0x12);
  i2c_write_register_add(device_add0,reg_add0,0xdb);//--set vcomh
  i2c_write_register_add(device_add0,reg_add0,0x40);//Set VCOM Deselect Level
  i2c_write_register_add(device_add0,reg_add0,0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
  i2c_write_register_add(device_add0,reg_add0,0x02);//
  i2c_write_register_add(device_add0,reg_add0,0x8d);//--set Charge Pump enable/disable
  i2c_write_register_add(device_add0,reg_add0,0x14);//--set(0x10) disable
  i2c_write_register_add(device_add0,reg_add0,0xa4);// Disable Entire Display On (0xa4/0xa5)
  i2c_write_register_add(device_add0,reg_add0,0xa6);// Disable Inverse Display On (0xa6/a7) 
  i2c_write_register_add(device_add0,reg_add0,0xaf);//--turn on oled panel
}

/********************************************
// 全屏填图，0x00为黑，0xFF为白
********************************************/
void fill_picture(unsigned char fill_Data)
{
  unsigned char m,n;
  for(m=0;m<8;m++)
  {
    i2c_write_register_add(device_add0,reg_add0,(0xb0+m));	//page0-page1
    i2c_write_register_add(device_add0,reg_add0,0x00);		//low column start address
    i2c_write_register_add(device_add0,reg_add0,0x10);		//high column start address
    i2c_Start_add();
    i2c_write_byte_add(device_add0);
    i2c_Wait_add();  
    i2c_write_byte_add(0x40);
    i2c_Wait_add();      
    for(n=0;n<128;n++)
    {
      i2c_write_byte_add(fill_Data);
      i2c_Wait_add();      
    }
    i2c_Stop_add();
    Delay_T3_uS(50);
  }
}

/********************************************
// 设置OLED起始点坐标
********************************************/
void OLED_Setxy(unsigned char x, unsigned char y)     //OLED 设置坐标  
{   
  i2c_write_register_add(device_add0,reg_add0,(0xb0+y));  
  i2c_write_register_add(device_add0,reg_add0,(((x&0xf0)>>4)|0x10));  
  i2c_write_register_add(device_add0,reg_add0,((x&0x0f)|0x01));   
} 

/********************************************
// 发送存在RAM的数据至OLED
********************************************/
void OLED_RAM_Refresh_add(void)
{
  unsigned char i, j;
  for(i = 0; i<8; i++)
  {
    OLED_Setxy(0,i);
    i2c_Start_add();
    i2c_write_byte_add(device_add0);
    i2c_Wait_add();  
    i2c_write_byte_add(0x40);
    i2c_Wait_add();  
    for(j = 0; j<128; j++)
    {
      i2c_write_byte_add(OLED_RAM_add[j][i]);
      i2c_Wait_add();
    }
    i2c_Stop_add();
    Delay_T3_uS(50);
  }
}

void OLED_RAM_Refresh_add_Line(uint8 line)		//line : 1 2 3 4
{
  unsigned char i , j;
  for(i = 2*(4-line); i < 2*(4-line)+2; i++)
  {
    OLED_Setxy(0,i);
    i2c_Start_add();
    i2c_write_byte_add(device_add0);
    i2c_Wait_add();  
    i2c_write_byte_add(0x40);
    i2c_Wait_add();  
    for(j = 0; j<128; j++)
    {
      i2c_write_byte_add(OLED_RAM_add[j][i]);
      i2c_Wait_add();
    }
    i2c_Stop_add();
    Delay_T3_uS(50);
  }
}

void OLED_RAM_Fill_add(unsigned char dot)
{
	unsigned char x,y;
	for(x =0; x<128;x++)
	{
		for(y = 0; y<8; y++)
		{
			if (dot)
				OLED_RAM_add[x][y] = 0xff;
			else
				OLED_RAM_add[x][y] = 0;
			
			
		}
	}
}

void OLED_RAM_Fill_add_ClearLine(uint8 line)
{
	unsigned char x,y;
	for(x =0; x<128;x++)
	{
		for(y = 2*(4-line); y<2*(4-line)+2; y++)
		{
			OLED_RAM_add[x][y] = 0;		
		}
	}
}


/********************************************
// 画点（列，行）
********************************************/
void OLED_RAM_DrawPoint_add(unsigned char x, unsigned char y, unsigned char t)
{
	unsigned char pos, bx, temp = 0;
	if(x>127 || y > 63) return ;
	x = 127 - x;
	y = 63 - y;
	pos = y/8;
	bx = y%8;
	temp = 1<<(bx);
	if(t) 	OLED_RAM_add[x][pos] |= temp;
	else 	OLED_RAM_add[x][pos] &=~temp;
}


/********************************************
// 显示0608
********************************************/
void OLED_RAM_ShowChar0608_add(unsigned char x, unsigned char y,
					unsigned char chr,
					unsigned char mode)
{
	unsigned char i,temp,j;
	chr = chr - ' ';
        EEPROM_middle_0608 = ((chr)<<3)+0x05f0;
        EEPROM_low_0608  = EEPROM_middle_0608&0x00ff;
        EEPROM_high_0608 = (EEPROM_middle_0608&0xff00)>>8; 
	for(i = 0; i<6; i++)
	{
		temp_test[i] = EEPROM_read_register_add(0xa0,EEPROM_high_0608,EEPROM_low_0608+i);
                temp = EEPROM_read_register_add(0xa0,EEPROM_high_0608,EEPROM_low_0608+i);
		for(j = 0; j<8; j++)
		{
			if(temp & 0x01)
			{
				OLED_RAM_DrawPoint_add(x+i,y+j,mode);
			}
			else 
			{
				OLED_RAM_DrawPoint_add(x+i,y+j,!mode);
			}
			temp>>=1;
		}
		
	}
}


/********************************************
// 显示0816
********************************************/
void OLED_RAM_ShowChar0816_add(unsigned char x, unsigned char y,
						unsigned char chr,
						unsigned char mode)
{
	unsigned char i, temp,j;
	chr = chr - ' ';
        EEPROM_middle_0816 = ((chr)<<4);
        EEPROM_low_0816  = EEPROM_middle_0816&0x00ff;
        EEPROM_high_0816 = (EEPROM_middle_0816&0xff00)>>8; 
	for(i = 0; i<8; i++)
	{
		temp = EEPROM_read_register_add(0xa0,EEPROM_high_0816,EEPROM_low_0816+i);
		for(j = 0; j<8; j++)
		{
			if(temp & 0x01)
			{
				OLED_RAM_DrawPoint_add(x+i,y+j,mode);
			}
			else 
			{
				OLED_RAM_DrawPoint_add(x+i,y+j,!mode);
			}
			temp>>=1;
		}
	}
	for(i = 0; i<8; i++)
	{  
		temp = EEPROM_read_register_add(0xa0,EEPROM_high_0816,EEPROM_low_0816+i+8);
		for(j = 0; j<8; j++)
		{
			if(temp & 0x01)
			{
				OLED_RAM_DrawPoint_add(x+i,y+j+8,mode);
			}
			else 
			{
				OLED_RAM_DrawPoint_add(x+i,y+j+8,!mode);
			}
			temp>>=1;
		}
	}	
}



/********************************************
// 显示string
********************************************/
void OLED_RAM_disp_string_add(unsigned char x,
					unsigned char y,
					const unsigned char * str,
					unsigned char size,
					unsigned char mode)
{
	while(*str != '\0')
	{
		switch(size)
		{
		case SIZE_0608:
			if(x>X_WIDTH-7){x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,*str,mode);
			x+=6;
			str++;
			break; 
		case SIZE_0816:
			if(x>X_WIDTH-9){x = 0; y+=16;}
			OLED_RAM_ShowChar0816_add(x,y,*str,mode);
			x+=8;
			str++;
		}
          }
}



/********************************************
// 显示number
********************************************/
void OLED_RAM_disp_num_add(unsigned char x,
					unsigned char y, 
					long long num,
					unsigned char size,
					unsigned char mode)
{
	unsigned char len = 0, value[20] = {0};
	long long temp;
	
	if(num<0)
	{
		switch(size)
		{
		case SIZE_0608:
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'-',mode);
			x += 6; 
			break ;
		case SIZE_0816:
			if(x > Y_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'-',mode);
			x += 8;
			break ;
		default :
			return;
		}
		num = -num;
	}

	temp = num;
	while(temp/10)
	{
		value[len] = temp % 10;
		len++;
		temp/=10;
	}
	value[len] = temp%10;
	for(int i = len; i>= 0; i--)
	{
		switch(size)
		{
		case SIZE_0608 :
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
			x += 6; 
			break;
		case SIZE_0816:
			if(x > X_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
			x += 8;
			break ;
		}
	}	
}

/********************************************
// 显示number 测试玩玩
********************************************/
void OLED_RAM_disp_num_test(unsigned char line,
                                        const unsigned char * str1,
                                        long num1,
                                        const unsigned char * str2,
                                        long num2,
					unsigned char size,
					unsigned char mode,
                                        int choice)
{
	unsigned char len = 0,value[20] = {0};
        unsigned char x,y;
	long temp;
        y=(line-1)*KSHOW_ROW_LINE;
        
        if(choice>3)
        {
          x=1;
          temp = choice;
	while(temp/10)
	{
		value[len] = temp % 10;
		len++;
		temp/=10;
	}
	value[len] = temp%10;
	for(int i = len; i>= 0; i--)
	{
		switch(size)
		{
		case SIZE_0608 :
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
			x += 6; 
			break;
		case SIZE_0816:
			if(x > X_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
			x += 8;
			break ;
		}
	}
        x=33;
        }
        else
        {
          x=41;
          OLED_RAM_disp_string_add(1,y,str1,FONT_SIZE_L,1);
        }
        
        if(choice==2)	
        {
             //High:110
	temp = num1;
	while(temp/10)
	{
		value[len] = temp % 10;
		len++;
		temp/=10;
	}
	value[len] = temp%10;
	for(int i = len; i>= 0; i--)
	{
		switch(size)
		{
		case SIZE_0608 :
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
			x += 6; 
			break;
		case SIZE_0816:
			if(x > X_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
			x += 8;
			break ;
		}
	}
        
        
          x=73;
          //High:110 Low:
          OLED_RAM_disp_string_add(x,y,str2,FONT_SIZE_L,1);
          x=105;
          temp = num2;
          len=0;
          while(temp/10)
          {
            value[len] = temp % 10;
            len++;
            temp/=10;
          }
          value[len] = temp%10;
          for(int i = len; i>= 0; i--)
          {
            switch(size)
            {
            case SIZE_0608 :
              if(x > X_WIDTH - 7) {x = 0; y += 8;}
              OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
              x += 6; 
              break;
            case SIZE_0816:
              if(x > X_WIDTH - 9)	{x = 0; y += 16;}
              OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
              x += 8;
              break ;
            }
          }	
        }
        else if(choice>3)	
        {
          x=41;
          //9999 Ds:**Dg:**
          OLED_RAM_disp_string_add(x,y,str1,FONT_SIZE_L,1);
          x=63;
          temp = num1;
          len=0;
          while(temp/10)
          {
            value[len] = temp % 10;
            len++;
            temp/=10;
          }
          value[len] = temp%10;
          for(int i = len; i>= 0; i--)
          {
            switch(size)
            {
            case SIZE_0608 :
              if(x > X_WIDTH - 7) {x = 0; y += 8;}
              OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
              x += 6; 
              break;
            case SIZE_0816:
              if(x > X_WIDTH - 9)	{x = 0; y += 16;}
              OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
              x += 8;
              break ;
            }
          }
          //
          x=79;
          //9999 Ds:**Dg:**
          OLED_RAM_disp_string_add(x,y,str2,FONT_SIZE_L,1);
          x=111;
          temp = num2;
          len=0;
          while(temp/10)
          {
            value[len] = temp % 10;
            len++;
            temp/=10;
          }
          value[len] = temp%10;
          for(int i = len; i>= 0; i--)
          {
            switch(size)
            {
            case SIZE_0608 :
              if(x > X_WIDTH - 7) {x = 0; y += 8;}
              OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
              x += 6; 
              break;
            case SIZE_0816:
              if(x > X_WIDTH - 9)	{x = 0; y += 16;}
              OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
              x += 8;
              break ;
            }
          }
          
          //
        }
        else
        {
          temp = num1;
	while(temp/10)
	{
		value[len] = temp % 10;
		len++;
		temp/=10;
	}
	value[len] = temp%10;
	for(int i = len; i>= 0; i--)
	{
		switch(size)
		{
		case SIZE_0608 :
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
			x += 6; 
			break;
		case SIZE_0816:
			if(x > X_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
			x += 8;
			break ;
		}
	}
        }
      
}

/********************************************
// 显示多个数  
********************************************/
void OLED_RAM_disp_num_five(unsigned char line,
                                        int num1,
                                        int num2,
                                        unsigned char num3,
                                        unsigned char num4,
                                        unsigned char num5,
					unsigned char size,
					unsigned char mode,
                                        unsigned char choice)
{

	unsigned char len = 0,value[20] = {0};
        unsigned char x,y;
	long temp;
        y=(line-1)*KSHOW_ROW_LINE;
	//第一个数 ****_ 占5位
        OLED_RAM_disp_num_add(1,y,num1,FONT_SIZE_L,1);
        //第二个数 ****_ 占5位
        x=41;
        //High:110
	temp = num2;
	while(temp/10)
	{
		value[len] = temp % 10;
		len++;
		temp/=10;
	}
	value[len] = temp%10;
	for(int i = len; i>= 0; i--)
	{
		switch(size)
		{
		case SIZE_0608 :
			if(x > X_WIDTH - 7) {x = 0; y += 8;}
			OLED_RAM_ShowChar0608_add(x,y,'0' + value[i],mode);
			x += 6; 
			break;
		case SIZE_0816:
			if(x > X_WIDTH - 9)	{x = 0; y += 16;}
			OLED_RAM_ShowChar0816_add(x,y,'0'+value[i],mode);
			x += 8;
			break ;
		}
	}
        //第三四五个数 *-*-* 占5位
        
       
        if(choice==88)
        {
          x=63;
          OLED_RAM_ShowChar0816_add(x,y,'O',1);
          x=71;
          OLED_RAM_ShowChar0816_add(x,y,'b',1);
          x=79;
          OLED_RAM_ShowChar0816_add(x,y,':',1);
          x=87;
          OLED_RAM_disp_num_add(x,y,num3,FONT_SIZE_L,1);
          x=103;
          OLED_RAM_disp_num_add(x,y,num4,FONT_SIZE_L,1);
          x=119;
          OLED_RAM_disp_num_add(x,y,num5,FONT_SIZE_L,1);
        }
         else if(choice==99)
        {
          x=63;
          OLED_RAM_ShowChar0816_add(x,y,'B',1);
          x=71;
          OLED_RAM_ShowChar0816_add(x,y,'r',1);
          x=79;
          OLED_RAM_ShowChar0816_add(x,y,':',1);
          x=87;
          OLED_RAM_disp_num_add(x,y,num3,FONT_SIZE_L,1);
          x=103;
          OLED_RAM_disp_num_add(x,y,num4,FONT_SIZE_L,1);
          x=119;
          OLED_RAM_disp_num_add(x,y,num5,FONT_SIZE_L,1);
        }
       else
        {
          x=81;
          OLED_RAM_disp_num_add(x,y,num3,FONT_SIZE_L,1);
          x=97;
          OLED_RAM_disp_num_add(x,y,num4,FONT_SIZE_L,1);
          x=113;
          OLED_RAM_disp_num_add(x,y,num5,FONT_SIZE_L,1);
        }
      
}

void OLED_Reset(unsigned char DATA)     //OLED 通过CH455重启 ,ox40重启，0x00恢复 
{
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(0x68);//CH455的设置DIG0命令丛机地址为0x68
    i2c_Wait_add();
    /* Write Register */
    i2c_write_byte_add(DATA); //复用SEG6管脚，0x80为高，0x00为低
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
    
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(0x6A);//CH455的设置DIG1命令丛机地址为0x6A
    i2c_Wait_add();
    /* Write Register */
    i2c_write_byte_add(DATA);//复用SEG6管脚，0x80为高，0x00为低
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
    
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(0x6C);//CH455的设置DIG2命令丛机地址为0x6C
    i2c_Wait_add();
    /* Write Register */
    i2c_write_byte_add(DATA);//复用SEG6管脚，0x80为高，0x00为低
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
    
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(0x6E);//CH455的设置DIG3命令丛机地址为0x6E
    i2c_Wait_add();
    /* Write Register */
    i2c_write_byte_add(DATA);//复用SEG6管脚，0x80为高，0x00为低
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
  
}

void Init_CH455()
{
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(0x48);//CH455的设置系统参数命令丛机地址为0x48
    i2c_Wait_add();
    /* Write Register */
    i2c_write_byte_add(0x39);//CH455的设置系统参数命令为 0[INTENS][7SEG][SLEEP]0[ENA]B。
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);


}

uint8 CH455_Key()
{
    uint8 get_key;

    /* start */
    i2c_Start_add();
    /* Send Slave Address */
    i2c_write_byte_add(0x4F);
    i2c_Wait_add();
    /* Put in Rx Mode */
    i2c_EnterRxMode_add();
    /* Turn off ACK since this is second to last byte being read*/
    i2c_DisableAck_add();
    /* Dummy read */
    i2c_read_byte_add(get_key); 
    i2c_Wait_add();
    /* Send stop since about to read last byte */
    i2c_Stop_add();
    /* Read byte */
    i2c_read_byte_add(get_key);

    Delay_T3_uS(50);

    switch(get_key)
    {
//#if PressStill
     /*以下为区分按键持续按下时的编码
        case 0x0F: return 0x00;
        case 0x04: return 0x01;
        case 0x0C: return 0x02;
        case 0x14: return 0x03;
        case 0x05: return 0x04;
        case 0x0D: return 0x05;
        case 0x15: return 0x06;
        case 0x06: return 0x07;
        case 0x0E: return 0x08;
        case 0x16: return 0x09;
        case 0x07: return 0x0a;
        case 0x17: return 0x0b;
        
        case 0x4F: return 0x80;
        case 0x44: return 0x81;
        case 0x4C: return 0x82;
        case 0x54: return 0x83;
        case 0x45: return 0x84;
        case 0x4D: return 0x85;
        case 0x55: return 0x86;
        case 0x46: return 0x87;
        case 0x4E: return 0x88;
        case 0x56: return 0x89;
        case 0x47: return 0x8a;
        case 0x57: return 0x8b;
#else   */ 
      
        /*以下为不区分按键持续按下时的编码*/
        case 0x0F: return 0x00;
        case 0x04: return 0x01;
        case 0x0C: return 0x02;
        case 0x14: return 0x03;
        case 0x05: return 0x04;
        case 0x0D: return 0x05;
        case 0x15: return 0x06;
        case 0x06: return 0x07;
        case 0x0E: return 0x08;
        case 0x16: return 0x09;
        case 0x07: return 0x0a;
        case 0x17: return 0x0b;
        
        case 0x4F: return 0x00;
        case 0x44: return 0x01;
        case 0x4C: return 0x02;
        case 0x54: return 0x03;
        case 0x45: return 0x04;
        case 0x4D: return 0x05;
        case 0x55: return 0x06;
        case 0x46: return 0x07;
        case 0x4E: return 0x08;
        case 0x56: return 0x09;
        case 0x47: return 0x0a;
        case 0x57: return 0x0b;
      
      
      
//////        case ~0x0F: return 0x00;
//////        case ~0x04: return 0x01;
//////        case ~0x0C: return 0x02;
//////        case ~0x14: return 0x03;
//////        case ~0x05: return 0x04;
//////        case ~0x0D: return 0x05;
//////        case ~0x15: return 0x06;
//////        case ~0x06: return 0x07;
//////        case ~0x0E: return 0x08;
//////        case ~0x16: return 0x09;
//////        case ~0x07: return 0x0a;
//////        case ~0x17: return 0x0b;
//////        
//////        case ~0x4F: return 0x00;
//////        case ~0x44: return 0x01;
//////        case ~0x4C: return 0x02;
//////        case ~0x54: return 0x03;
//////        case ~0x45: return 0x04;
//////        case ~0x4D: return 0x05;
//////        case ~0x55: return 0x06;
//////        case ~0x46: return 0x07;
//////        case ~0x4E: return 0x08;
//////        case ~0x56: return 0x09;
//////        case ~0x47: return 0x0a;
//////        case ~0x57: return 0x0b;

//#endif       
        /*以下为组合按键的编码
        case 0x7F: return 0x1f;//同时按下*和0
        case 0x7E: return 0x1e;//同时按下7和8
        case 0x7D: return 0x1d;//同时按下4和5
        case 0x7C: return 0x1c;//同时按下1和2

        case 0x3F: return 0x1f;//同时按下*和0（同时释放）
        case 0x3E: return 0x1e;//同时按下7和8（同时释放）
        case 0x3D: return 0x1d;//同时按下4和q5（同时释放）
        case 0x3C: return 0x1c;//同时按下1和2（同时释放）
*/        
        default:   return 0xFF;
    }
    
}
#endif

