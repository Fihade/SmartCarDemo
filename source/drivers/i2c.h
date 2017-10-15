/*
 * File:	i2c.h
 * Purpose:	Inter-Integrated Circuit
 *
 * Notes:		
 */

#ifndef __I2C_H__
#define __I2C_H__


#define IIC_SDA_PIN   (3)
#define IIC_SDA_DOR   GPIOB_PDOR
#define IIC_SDA_DIR   GPIOB_PDIR
#define IIC_SDA_DDR   GPIOB_PDDR
#define IIC_SDA_PCR   PORTB_PCR3

#define IIC_SCK_PIN   (2)
#define IIC_SCK_DOR   GPIOB_PDOR
#define IIC_SCK_DDR   GPIOB_PDDR
#define IIC_SCK_PCR   PORTB_PCR2

#define IIC_SDA_HIGH()    SET_BIT(IIC_SDA_DOR, IIC_SDA_PIN)
#define IIC_SDA_LOW()     CLR_BIT(IIC_SDA_DOR, IIC_SDA_PIN)
#define IIC_SDA_STATUS()  BIT_IS_HIGH(IIC_SDA_DIR, IIC_SDA_PIN)
#define IIC_SCK_HIGH()    SET_BIT(IIC_SCK_DOR, IIC_SCK_PIN)
#define IIC_SCK_LOW()     CLR_BIT(IIC_SCK_DOR, IIC_SCK_PIN)



/******************************************************************************/

// Function prototypes
void i2c_start(void);
void i2c_stop(void);
void i2c_write_byte(uint8 out_byte);
uint8 i2c_read_byte(void);
uint8 GetAck(void);
void PutAck(uint8 ack);

void i2c_init(void);
void i2c_write_register(uint8 register_address, uint8 data);
void i2c_write_reg2byte(uint8 register_address, uint8 data1, uint8 data2);
void i2c_write_regNsame(uint8 register_address, uint8 data, uint8 n);
uint8 i2c_read_register(uint8 register_address);

/******************************************************************************/

// Config
#define DEVICE_ADDRESS  (0x70)//(0x4c<<1) // the address of the i2c device

/******************************************************************************/
#endif /* __I2C_H__ */
