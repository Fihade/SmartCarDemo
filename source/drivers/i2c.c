/*
 * File:	i2c.c
 * Purpose:	Inter-Integrated Circuit
 *
 * Notes:		
 */

#include "common.h"
#include "i2c.h"
/******************************************************************************/
/*!
 * I2C Initialization
 * Set Baud Rate and turn on I2C0
 */
uint8 ack = 0;

void i2c_init(void)
{
    IIC_SDA_PCR = (0 | PORT_PCR_MUX(1));
    IIC_SCK_PCR = (0 | PORT_PCR_MUX(1));
    SET_BIT(IIC_SCK_DDR, IIC_SCK_PIN);
    SET_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    
    IIC_SDA_HIGH();
    IIC_SCK_HIGH();
    
}

void i2c_start(void)
{
    IIC_SDA_HIGH();
    IIC_SCK_HIGH();
    
    Delay_T3_uS(50);
    IIC_SDA_LOW();
    Delay_T3_uS(50);
    IIC_SCK_LOW();
    Delay_T3_uS(50);
}

void i2c_stop(void)
{
    IIC_SDA_LOW();
    Delay_T3_uS(50);
    IIC_SCK_HIGH();
    Delay_T3_uS(50);
    IIC_SDA_HIGH();
    Delay_T3_uS(50);
}

void i2c_write_byte(uint8 out_byte)
{
    uint8 i = 0;
    IIC_SCK_LOW();
    
    for(i = 0; i < 8; i++)
    {
        if( (out_byte << i) & 0x80 )
        {
            IIC_SDA_HIGH();
        }else
        {
            IIC_SDA_LOW();
        }
        Delay_T3_uS(50);
        IIC_SCK_HIGH();
        Delay_T3_uS(50);
        IIC_SCK_LOW();
    }
    Delay_T3_uS(50);
    IIC_SDA_HIGH();
    Delay_T3_uS(50);
    IIC_SCK_HIGH();
    CLR_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    Delay_T3_uS(50);
    if(IIC_SDA_STATUS())
    {
        ack = 0 ;
    }else{
        ack = 1 ;        /*判断是否接收到应答信号*/
    }
    SET_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    IIC_SCK_LOW();
}

uint8 i2c_read_byte(void)
{
    uint8 i ;
    uint8 rec_byte = 0 ;
    
    IIC_SDA_HIGH();
    Delay_T3_uS(50) ;
    CLR_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    Delay_T3_uS(50) ;
    for(i = 0; i < 8; i++)
    {
        IIC_SCK_LOW();
        Delay_T3_uS(50) ;
        IIC_SCK_HIGH();
        rec_byte <<= 1 ;
        if(IIC_SDA_STATUS())
        {
            rec_byte += 0x01 ;
        }
        Delay_T3_uS(50) ;
    }
    IIC_SCK_LOW();
    // 产生一个非应答
    PutAck(1) ;
    
    SET_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    return rec_byte ;
}

uint8 GetAck(void)
{
    uint8 ack;

    IIC_SCK_LOW();
    IIC_SDA_HIGH();
    Delay_T3_uS(50) ;
    CLR_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    Delay_T3_uS(50) ;

    IIC_SCK_HIGH();
    Delay_T3_uS(50) ;
    ack = IIC_SDA_STATUS();
    Delay_T3_uS(50) ;
    if(ack) ack = 1;

    SET_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    return ack ;
}

void PutAck(uint8 ack)
{
    if (ack)
    {
        SET_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    }else
    {
        CLR_BIT(IIC_SDA_DDR, IIC_SDA_PIN);
    }

    Delay_T3_uS(50) ;
    IIC_SCK_HIGH();
    Delay_T3_uS(50) ;
    IIC_SCK_LOW();
    Delay_T3_uS(50) ;
}

/******************************************************************************/
void i2c_write_register(uint8 register_address, uint8 data)
{
    /* start */
    i2c_start();
    /* send data to slave */
    i2c_write_byte(DEVICE_ADDRESS & 0xfe);
    /* Write Register Address */
    i2c_write_byte(register_address);
    /* Write data */
    i2c_write_byte(data);
    /* stop */
    i2c_stop();
    Delay_T3_uS(50);
}

/******************************************************************************/
void i2c_write_reg2byte(uint8 register_address, uint8 data1, uint8 data2)
{
    /* start */
    i2c_start();
    /* send data to slave */
    i2c_write_byte(DEVICE_ADDRESS & 0xfe);
    /* Write Register Address */
    Delay_T3_uS(500);
    i2c_write_byte(register_address);
    /* Write data */
    i2c_write_byte(data1);
    /* Write data */
    i2c_write_byte(data2);
    /* stop */
    i2c_stop();
    Delay_T3_uS(50);
}

/******************************************************************************/
void i2c_write_regNsame(uint8 register_address, uint8 data, uint8 n)
{
    /* start */
    i2c_start();
    /* send data to slave */
    i2c_write_byte(DEVICE_ADDRESS & 0xfe);
    /* Write Register Address */
    i2c_write_byte(register_address);
    /* Write data */
    while(n--)
    {
        i2c_write_byte(data);
    }
    /* stop */
    i2c_stop();
    Delay_T3_uS(50);
}

/******************************************************************************/
uint8 i2c_read_register(uint8 register_address)
{
    uint8 rec_data = 0;
    /* start */
    i2c_start();
    /* Send Slave Address */
    i2c_write_byte(DEVICE_ADDRESS & 0xfe);
    /* Write Register Address */
    i2c_write_byte(register_address);
    /* Do a repeated start */
    i2c_start();
    /* Send Slave Address */
    i2c_write_byte(DEVICE_ADDRESS | 0x01);
    /* Turn off ACK since this is second to last byte being read*/
    /* Dummy read */
    rec_data = i2c_read_byte(); 
    /* Send stop since about to read last byte */
    i2c_stop();
    /* Read byte */
    
    return rec_data;
}

/******************************************************************************/
