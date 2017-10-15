/*
 * File:	spi.c
 * Purpose:	communication with LCD by spi
 *
 * Notes:		
 */
#include "common.h"
#include "spi.h"

#if(0==MASTOR0_SLAVE2)
/******************************************************************************/
/*
 * Master Mode, PLL150: 4kB/mS
 */
void spi_init(void)
{
    // port init
    SPI_PCS_PCR = PORT_PCR_MUX(SPI_PCS_ALTn);
    SPI_SOUT_PCR = PORT_PCR_MUX(SPI_SOUT_ALTn);
    SPI_SCK_PCR = PORT_PCR_MUX(SPI_SCK_ALTn);
    SPI_SIN_PCR = PORT_PCR_MUX(SPI_SIN_ALTn);
    
    SPI_CP1_PCR = PORT_PCR_MUX(1); // gpio
    SPI_SLAVE_END(); // clear
    SET_BIT(SPI_CP1_PDDR, SPI_CP1_PIN); // output

    // open the clock gate of spi
#if(0==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_DSPI0_MASK;
#elif(1==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
#elif(2==SPI_CHANNEL)
    SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;
#else
#error "Definition of SPI_CHANNEL ERROR ! "
#endif
    
    // Module Configuration Register
    SPI_MCR_REG(SPI_PTR) = SPI_MCR_MSTR_MASK
                         | SPI_MCR_DCONF(0x0)
                         | SPI_MCR_PCSIS(0x0)
                         | SPI_MCR_HALT_MASK;
    // Flushes the TX FIFO, Flushes the RX FIFO
    SPI_MCR_REG(SPI_PTR) |= SPI_MCR_CLR_TXF_MASK
                          | SPI_MCR_CLR_RXF_MASK;
    // Frame Size and baud rate
    SPI_CTAR_REG(SPI_PTR,0) = SPI_CTAR_FMSZ(SPI_FRAME_SIZE)
                            | SPI_CTAR_DBR_MASK;
    
    SPI_CTAR_REG(SPI_PTR,1) = SPI_CTAR_FMSZ(SPI_FRAME_SIZE)
                            | SPI_CTAR_DBR_MASK;
    // clear all flag
    SPI_SR_REG(SPI_PTR) = 0xffff0000UL;
    // close all interrupt
    SPI_RSER_REG(SPI_PTR) = 0x00UL;
    // start DSPI transfers
    SPI_MCR_REG(SPI_PTR) &= ~SPI_MCR_HALT_MASK;
}

/******************************************************************************/
void spi_send_vector(SPI_TYPE *p, int n)
{
    static uint32 command = SPI_PUSHR_CTAS(0)|SPI_PUSHR_PCS(SPI_PCS_NUM);
    /*
    // Flushes the TX FIFO, Flushes the RX FIFO
    SPI_MCR_REG(SPI_PTR) |= SPI_MCR_CLR_TXF_MASK
                          | SPI_MCR_CLR_RXF_MASK;
    // clear all flag
    SPI_SR_REG(SPI_PTR) = 0xffff0000UL;
    */
    while(n-- > 0)
    {
        while(!(SPI_SR_REG(SPI_PTR)&SPI_SR_TFFF_MASK));
        
        if(n > 0)
            SPI_PUSHR_REG(SPI_PTR) = command | *p++;
        else
            SPI_PUSHR_REG(SPI_PTR) = command | *p++ | SPI_PUSHR_EOQ_MASK;
        
        SPI_SR_REG(SPI_PTR) |= SPI_SR_TFFF_MASK;
    }
}

/******************************************************************************/
void spi_init_ForLCD(void)
{
    // port init
    SPI_PCS_PCR = PORT_PCR_MUX(SPI_PCS_ALTn);
    SPI_SOUT_PCR = PORT_PCR_MUX(SPI_SOUT_ALTn);
    SPI_SCK_PCR = PORT_PCR_MUX(SPI_SCK_ALTn);
    SPI_SIN_PCR = PORT_PCR_MUX(SPI_SIN_ALTn);
    
    SPI_CP1_PCR = PORT_PCR_MUX(1); // gpio
    SPI_SLAVE_END(); // clear
    SET_BIT(SPI_CP1_PDDR, SPI_CP1_PIN); // output

    // open the clock gate of spi
#if(0==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_DSPI0_MASK;
#elif(1==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
#elif(2==SPI_CHANNEL)
    SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;
#else
#error "Definition of SPI_CHANNEL ERROR ! "
#endif
    
    // Module Configuration Register
    SPI_MCR_REG(SPI_PTR) = SPI_MCR_MSTR_MASK
                         | SPI_MCR_DCONF(0x0)
                         | SPI_MCR_PCSIS(0x0)
                         | SPI_MCR_HALT_MASK;
    // Flushes the TX FIFO, Flushes the RX FIFO
    SPI_MCR_REG(SPI_PTR) |= SPI_MCR_CLR_TXF_MASK
                          | SPI_MCR_CLR_RXF_MASK;
    // Frame Size and baud rate
    SPI_CTAR_REG(SPI_PTR,0) = SPI_CTAR_FMSZ(SPI_FRAME_SIZE) | SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK
                             | SPI_CTAR_DBR_MASK | SPI_CTAR_BR(2); // baud rate
    SPI_CTAR_REG(SPI_PTR,1) = SPI_CTAR_FMSZ(SPI_FRAME_SIZE) | SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK
                             | SPI_CTAR_DBR_MASK | SPI_CTAR_BR(2); // baud rate
    
    // clear all flag
    SPI_SR_REG(SPI_PTR) = 0xffff0000UL;
    // close all interrupt
    SPI_RSER_REG(SPI_PTR) = 0x00UL;
    // start DSPI transfers
    SPI_MCR_REG(SPI_PTR) &= ~SPI_MCR_HALT_MASK;
}
/******************************************************************************/
void spi_send_vector_ForLCD(SPI_TYPE *p, int n)
{
    static uint32 command = SPI_PUSHR_CTAS(0)|SPI_PUSHR_PCS(SPI_PCS_NUM);

    while(n-- > 0)
    {
        while(!(SPI_SR_REG(SPI_PTR)&SPI_SR_TFFF_MASK))Delay_T3_uS(1);
        
        if(n > 0)
            SPI_PUSHR_REG(SPI_PTR) = command | *p++;
        else
            SPI_PUSHR_REG(SPI_PTR) = command | *p++ | SPI_PUSHR_EOQ_MASK;
        //Delay_T3_uS(1);
        SPI_SR_REG(SPI_PTR) |= SPI_SR_TFFF_MASK;
    }
}
/******************************************************************************/
void spi_send_byte_ForLCD(SPI_TYPE data)
{
    static uint32 command = SPI_PUSHR_CTAS(0)|SPI_PUSHR_PCS(SPI_PCS_NUM);

    while(!(SPI_SR_REG(SPI_PTR)&SPI_SR_TFFF_MASK))Delay_T3_uS(1);
    
    SPI_PUSHR_REG(SPI_PTR) = command | data;
    
    SPI_SR_REG(SPI_PTR) |= SPI_SR_TFFF_MASK;
}

/******************************************************************************/

#elif(2==MASTOR0_SLAVE2) // #if(0==MASTOR0_SLAVE2)
/******************************************************************************/
/*
 * Slave Mode
 */
void spi_init(void)
{
    // port init
    SPI_PCS_PCR = PORT_PCR_MUX(SPI_PCS_ALTn);
    SPI_SOUT_PCR = PORT_PCR_MUX(SPI_SOUT_ALTn);
    SPI_SCK_PCR = PORT_PCR_MUX(SPI_SCK_ALTn);
    SPI_SIN_PCR = PORT_PCR_MUX(SPI_SIN_ALTn);
    
    // open the clock gate of spi
#if(0==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_DSPI0_MASK;
#elif(1==SPI_CHANNEL)
    SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
#elif(2==SPI_CHANNEL)
    SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;
#else
#error "Definition of SPI_CHANNEL ERROR ! "
#endif
    
    // Module Configuration Register
    SPI_MCR_REG(SPI_PTR) = SPI_MCR_DCONF(0x0)
                         | SPI_MCR_PCSIS(0x0)
                         | SPI_MCR_HALT_MASK;
    // Flushes the TX FIFO, Flushes the RX FIFO
    SPI_MCR_REG(SPI_PTR) |= SPI_MCR_CLR_TXF_MASK
                          | SPI_MCR_CLR_RXF_MASK;
    // Frame Size
    SPI_CTAR_SLAVE_REG(SPI_PTR,0) = SPI_CTAR_SLAVE_FMSZ(SPI_FRAME_SIZE);
    SPI_CTAR_SLAVE_REG(SPI_PTR,1) = SPI_CTAR_SLAVE_FMSZ(SPI_FRAME_SIZE);
    // clear all flag
    SPI_SR_REG(SPI_PTR) = 0xffff0000UL;
    // close all interrupt
    SPI_RSER_REG(SPI_PTR) = 0x00UL;
    // start DSPI transfers
    SPI_MCR_REG(SPI_PTR) &= ~SPI_MCR_HALT_MASK;
}

/******************************************************************************/
void spi_recieve_vector(SPI_TYPE *p, int n)
{
    // Flushes the TX FIFO, Flushes the RX FIFO
    SPI_MCR_REG(SPI_PTR) |= SPI_MCR_CLR_TXF_MASK
                          | SPI_MCR_CLR_RXF_MASK;
    // clear all flag
    SPI_SR_REG(SPI_PTR) = 0xffff0000UL;
    while(n-- > 0)
    {
        while(!(SPI_SR_REG(SPI_PTR)&SPI_SR_RFDF_MASK));
        *p++ = (SPI_TYPE)SPI_POPR_REG(SPI_PTR);
        SPI_SR_REG(SPI_PTR) |= SPI_SR_RFDF_MASK;
    }
}
/******************************************************************************/
#endif // #eif(2==MASTOR0_SLAVE2)
