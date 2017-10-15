#ifndef _SCCB_H__
#define _SCCB_H__

/*--------------- I N C L U D E S ------------------------------------*/
#include "common.h"

/*-------------- - M A C R O S ----------------------------------------*/
/*
	SCL		PE1
	SDA		PE0
*/
#define SCCB_SCL_PIN	(1)
#define SCCB_SCL_PDOR	GPIOE_PDOR
#define SCCB_SCL_PDDR	GPIOE_PDDR
#define SCCB_SCL_PCR	PORTE_PCR1

#define SCCB_SDA_PIN	(0)
#define SCCB_SDA_PDOR	GPIOE_PDOR
#define SCCB_SDA_PDIR	GPIOE_PDIR
#define SCCB_SDA_PDDR	GPIOE_PDDR
#define SCCB_SDA_PCR	PORTE_PCR0

#define SCCB_SIC_BIT     (1)
#define SCCB_SID_BIT     (0)

#define SCCB_SIC_H()     SET_BIT(SCCB_SCL_PDOR, SCCB_SIC_BIT)
#define SCCB_SIC_L()     CLR_BIT(SCCB_SCL_PDOR, SCCB_SIC_BIT)

#define SCCB_SID_H()     SET_BIT(SCCB_SDA_PDOR, SCCB_SID_BIT)
#define SCCB_SID_L()     CLR_BIT(SCCB_SDA_PDOR, SCCB_SID_BIT)

#define SCCB_SID_IN      CLR_BIT(SCCB_SDA_PDDR, SCCB_SDA_PIN)
#define SCCB_SID_OUT     SET_BIT(SCCB_SDA_PDDR, SCCB_SDA_PIN)

#define SCCB_SID_STATE	 BIT_IS_HIGH(SCCB_SDA_PDIR, SCCB_SID_BIT)

#define MT9v022 	0x90

/*----------------------- D E C L A R A T I O N ------------------------------*/
unsigned char sccb_init();
unsigned char mt9v022_init(void);

unsigned short sccb_read(unsigned char regID);
unsigned char sccb_write(unsigned int regID, unsigned int regDat);

#endif
