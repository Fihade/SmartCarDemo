/*
 * File:	spi.h
 * Purpose:	communication with LCD by spi
 *
 * Notes:		
 */

#ifndef __SPI_H__
#define __SPI_H__

/******************************************************************************/

// Definition
#define SPI_CHANNEL     (0) // 0-2
#define SPI_PTR         SPI0_BASE_PTR // SPI0_BASE_PTR - SPI2_BASE_PTR

#define SPI_PCS_PCR     PORTA_PCR14
#define SPI_PCS_ALTn    (2)
#define SPI_PCS_NUM     (0) // 0-4

#define SPI_SOUT_PCR    PORTA_PCR16
#define SPI_SOUT_ALTn   (2)
#define SPI_SCK_PCR     PORTA_PCR15
#define SPI_SCK_ALTn    (2)
#define SPI_SIN_PCR     PORTA_PCR17
#define SPI_SIN_ALTn    (2)

#define SPI_FRAME_SIZE  (7) // SPI_FRAME_SIZE+1 is equal to the number of bits transferred per frame
#define SPI_TYPE        uint8

#define SPI_CP1_PCR     PORTA_PCR13
#define SPI_CP1_PDDR    GPIOA_PDDR
#define SPI_CP1_PDOR    GPIOA_PDOR
#define SPI_CP1_PIN     (13)

#define SPI_CP2_PCR     PORTA_PCR12
#define SPI_CP2_PIN     (12)


#define MASTOR0_SLAVE2  (0)
// Interface
// Function prototypes
#if(0==MASTOR0_SLAVE2)

#define SPI_SLAVE_START()   SET_BIT(SPI_CP1_PDOR, SPI_CP1_PIN);Delay_T3_uS(3)
#define SPI_SLAVE_END()     CLR_BIT(SPI_CP1_PDOR, SPI_CP1_PIN)

void spi_init(void);
void spi_send_vector(SPI_TYPE *p, int n);
void spi_init_ForLCD(void);
void spi_send_vector_ForLCD(SPI_TYPE *p, int n);
void spi_send_byte_ForLCD(SPI_TYPE data);

#elif(2==MASTOR0_SLAVE2) // #if(0==MASTOR0_SLAVE2)

void spi_init(void);
void spi_recieve_vector(SPI_TYPE *p, int n);

#else // #elif(2==MASTOR0_SLAVE2)
#error"Definition of MASTOR0_SLAVE2 ERROR ! "
#endif //#if(0==MASTOR0_SLAVE2) 

/******************************************************************************/
#endif /* __SPI_H__ */
