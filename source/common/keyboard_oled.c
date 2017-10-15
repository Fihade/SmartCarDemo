/*
* File:	keyboard.c
* Purpose: keyboard ZLG7290
*
* Notes:
*/
#if(1)
#include "common.h"
#include "keyboard_oled.h"

// 全局变量

/******************************************************************************/
uint16 ZLG7290GetKeyAndDisplay(void)
{
  uint8 i,tmp = 0;
  uint16 rt = 0;        
  
  KShowLineInputClr();
  
  for(i = 0;i < 8;)
  {
    while(ZLG7290_INT()==1);
    tmp = CH455_Key();   
    if(tmp == 0x0a)
    {
      i=0;
      rt = 0;
      KShowLineInputClr();
      while(!(ZLG7290_INT()==1));
      continue;
    }
    else if(tmp == 0x0b) return rt;
    else
    {
      if(rt * 10 + tmp > 65000)
      {
        continue;
      }
      rt = rt * 10 + tmp;
      i++;
      KShowLineNumInput(rt);
    }
    while(!(ZLG7290_INT()==1));
  }
  
  return rt;
}

void Choise(void)
{
  //初始化
  Delay_T3_mS(150);
  Init_OLED();
  Init_CH455(); 
  OLED_Reset(0x00);
  OLED_RAM_Fill_add(0);  
  OLED_RAM_Refresh_add();

  INT16U choise;
  Delay_T3_mS(50);
  
  for (;;)
  {	
    OLED_RAM_Fill_add(0);  
    OLED_RAM_Refresh_add();
    
    KShowLineStr(1,"HELLO SMARTCAR");

    choise = (ZLG7290GetKeyAndDisplay());
    Delay_T3_mS(10);
    
    if(choise == 0x00)         //开始发车
    {
      OLED_RAM_Fill_add(0);  
      OLED_RAM_Refresh_add();
      Delay_T3_mS(50);
      break;
    }
    else if(choise == 1)       
    {
      Set_Mid();
    }
    else
    {
      //NOTHING;
    }
  }

  KShowLineStr(4,"Go......");
  
}


void Set_Mid(void)
{
  INT16U SM_temp = PWM_SVO_MIDDLE;
  
  KShowLineStr(2,"ServoTest");
  
  KShowLineNumInfo(PWM_SVO_MIDDLE);
  
  for(;;)
  {
    SM_temp = ZLG7290GetKeyAndDisplay();
    SET_PWM_SVO((INT16S)SM_temp);
    
    Delay_T3_mS(50);
    KShowLineNumInfo(SM_temp);
    Delay_T3_mS(50);
  }
}
#endif
