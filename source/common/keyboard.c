/*
 * File:	keyboard.c
 * Purpose: keyboard ZLG7290
 *
 * Notes:
 */
#if(0)
#include "common.h"
#include "keyboard.h"

// 全局变量
// 外部变量
/******************************************************************************/

/******************************************************************************/
inline uint8 SwitchNumber(uint8 key_code)
{
    switch(key_code)
    {
        case 0x09: return 0;
        case 0x04: return 1;
        case 0x0C: return 2;
        case 0x14: return 3;
        case 0x03: return 4;
        case 0x0B: return 5;
        case 0x13: return 6;
        case 0x02: return 7;
        case 0x0A: return 8;
        case 0x12: return 9;
        case 0x01: return 0x0a;
        case 0x11: return 0x0b;
        default:   return 0xff;
    }
} // SwitchNumber

/******************************************************************************/
inline void ZLG7290ClearAll(void)
{
    i2c_write_regNsame(0x10, 0x00, 8);
    i2c_write_reg2byte(0x07, 0x70, 0x00);
} // ZLG7290ClearAll

/******************************************************************************/
inline void ZLG7290ClearHalf(void)
{
    i2c_write_regNsame(0x10, 0x00, 4);
    i2c_write_reg2byte(0x07, 0x70, 0x00);
} // ZLG7290ClearHalf

/******************************************************************************/
void ZLG7290DisplayData(uint32 data,uint8 bit)
{
    uint8 i;
    uint32 k = 10000000;
    for(i = 0;i < 8;i++)
    {
        if(i >= (8 - bit))
            i2c_write_reg2byte(0x07,0x60|i,((uint8)(data / k)) & 0x0F);

        data %= k;
        k /= 10;
    }
} // ZLG7290DisplayData

/******************************************************************************/
void ZLG7290UpDisplayData(uint32 data,uint8 bit)
{
    uint8 i;
    uint32 k = 1000;
    for(i = 4;i < 8;i++)
    {
        if(i >= (8 - bit))
            i2c_write_reg2byte(0x07,0x60|i,((uint8)(data / k)) & 0x0F);

        data %= k;
        k /= 10;
    }
}

/******************************************************************************/
void ZLG7290DownDisplayData(uint32 data,uint8 bit)
{
    uint8 i;
    uint32 k = 1000;
    for(i = 0;i < 4;i++)
    {
        if(i >= (4 - bit))
            i2c_write_reg2byte(0x07,0x60|i,((uint8)(data / k)) & 0x0F);
        data %= k;
        k /= 10;
    }
}

/******************************************************************************/
uint16 ZLG7290GetKeyAndDisplay(void)
{
    uint8 i,tmp = 0;
    uint16 rt = 0;

    for(i = 0;i < 8;)
    {

        while(ZLG7290_INT()==1);
        tmp = SwitchNumber(i2c_read_register(0x01));
        if(tmp == 0x0a)
        {
          i=0;
          rt = 0;
          ZLG7290ClearHalf();
          while(!(ZLG7290_INT()==1));
          continue;
        }
        else if(tmp == 0x0b) return rt;
        else
        {
          i2c_write_reg2byte (0x07,0x60|i,tmp);
          rt = rt * 10 + tmp;
          i++;
        }
        while(!(ZLG7290_INT()==1));
    }

    return rt;
}



#endif