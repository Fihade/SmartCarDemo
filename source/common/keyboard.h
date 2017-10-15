/*
 * File:	keyboard.h
 * Purpose:	keyboard ZLG7290
 *
 * Notes:		
 */
#if(0)
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/******************************************************************************/
// ºê¶¨Òå

// Function prototypes
void ZLG7290DisplayData(uint32 data,uint8 bit);
void ZLG7290UpDisplayData(uint32 data,uint8 bit);
void ZLG7290DownDisplayData(uint32 data,uint8 bit);
void ZLG7290ClearAll(void);
uint16 ZLG7290GetKeyAndDisplay(void);

// ²ßÂÔ



/******************************************************************************/

#endif /* __KEYBOARD_H__ */
#endif
