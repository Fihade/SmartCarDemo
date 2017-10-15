/*
 * File:	keyboard.h
 * Purpose:	keyboard ZLG7290
 *
 * Notes:		
 */
#if(1)
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/******************************************************************************/
// 宏定义
#define KSHOW_COL_ST		1
#define	KSHOW_ROW_LINE		16
#define	KSHOW_LINE_INFO		3
#define	KSHOW_LINE_INPUT	4

#define	KShowLineClear(line)	{OLED_RAM_Fill_add_ClearLine(line);\
				 OLED_RAM_Refresh_add_Line(line);}

#define	KShowLineStr(line,str)	{OLED_RAM_Fill_add_ClearLine(line);\
				 OLED_RAM_disp_string_add(KSHOW_COL_ST,(line-1)*KSHOW_ROW_LINE,str,FONT_SIZE_L,1);\
				 OLED_RAM_Refresh_add_Line(line);}

#define KShowLineNum(line,num)	{OLED_RAM_Fill_add_ClearLine(line);\
				 OLED_RAM_disp_num_add(KSHOW_COL_ST,(line-1)*KSHOW_ROW_LINE,num,FONT_SIZE_L,1);\
				 OLED_RAM_Refresh_add_Line(line);}

#define	KShowLineStrInput(str)	KShowLineStr(KSHOW_LINE_INPUT,str)
#define	KShowLineNumInput(num)	KShowLineNum(KSHOW_LINE_INPUT,num)
#define	KShowLineInputClr()	KShowLineClear(KSHOW_LINE_INPUT)
#define	KShowLineStrInfo(str)	KShowLineStr(KSHOW_LINE_INFO,str)
#define	KShowLineNumInfo(num)	KShowLineNum(KSHOW_LINE_INFO,num)
#define KShowLineInfoClr()	KShowLineClear(KSHOW_LINE_INFO)

// 外部变量

//Function
uint16 ZLG7290GetKeyAndDisplay(void);

// 策略
void Choise(void);              // 0 GO
void Set_StopTimer(void);
void Set_Mid(void);

/******************************************************************************/

#endif /* __KEYBOARD_H__ */
#endif