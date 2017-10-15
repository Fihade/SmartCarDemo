/*
 * File:	i2chard.h
 * Purpose:	Inter-Integrated Circuit
 *
 * Notes:		
 */
#if(1)
#ifndef __I2CHARD_H__
#define __I2CHARD_H__

/******************************************************************************/
// Function prototypes
extern void i2c_init_add(void);
extern void i2c_write_register_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data);
extern void i2c_write_reg2byte_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data1, uint8 data2);
extern void i2c_write_regNsame_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data, uint8 n);
extern uint8 i2c_read_register_add(uint8 DEVICE_ADDRESS_add, uint8 register_address);

/******************************************************************************/
#define I2C_BAUD_ICR_ADD    (0x20) // I2C baud rate config, determine the Divider factor
//#define DEVICE_ADDRESS  (0x70) //(0x70) // the address of the i2c device

// Definition
#define I2C_CHANNEL     (0) // 0-1
#define I2C_CHAN_PTR    I2C0_BASE_PTR // I2C0_BASE_PTR - I2C1_BASE_PTR        //0

#define I2C_SCL_PCR     PORTB_PCR2        //PTB2
#define I2C_SCL_ALTn    (2)               //2
#define I2C_SDA_PCR     PORTB_PCR3        //PTB3
#define I2C_SDA_ALTn    (2)               //2

/******************************************************************************/

/******************************************************************************/
#define i2c_Start_add()             {I2C_C1_REG(I2C_CHAN_PTR) |= I2C_C1_TX_MASK;\
                                 I2C_C1_REG(I2C_CHAN_PTR) |= I2C_C1_MST_MASK;}

#define i2c_Stop_add()              {I2C_C1_REG(I2C_CHAN_PTR) &= ~I2C_C1_MST_MASK;\
                                 I2C_C1_REG(I2C_CHAN_PTR) &= ~I2C_C1_TX_MASK;}

//#define i2c_Wait_add()              {while((I2C_S_REG(I2C_CHAN_PTR) & I2C_S_IICIF_MASK)==0);\
//                                 I2C_S_REG(I2C_CHAN_PTR) |= I2C_S_IICIF_MASK;}

#define i2c_write_byte_add(data)    I2C_D_REG(I2C_CHAN_PTR) = (data);

#define i2c_read_byte_add(data)     data = I2C_D_REG(I2C_CHAN_PTR);

#define i2c_RepeatedStart_add()     I2C_C1_REG(I2C_CHAN_PTR) |= I2C_C1_RSTA_MASK;

#define i2c_DisableAck_add()        I2C_C1_REG(I2C_CHAN_PTR) |= I2C_C1_TXAK_MASK;

#define i2c_EnterRxMode_add()       {I2C_C1_REG(I2C_CHAN_PTR) &= ~I2C_C1_TX_MASK;\
                                 I2C_C1_REG(I2C_CHAN_PTR) &= ~I2C_C1_TXAK_MASK;}

extern void i2c_Wait_add();
/******************************************************************************/
extern void EEPROM_write_register_add(uint8 DEVICE_ADDRESS_add, uint8 high_address, uint8 low_address, uint8 data);
extern uint8 EEPROM_read_register_add(uint8 DEVICE_ADDRESS_add, uint8 high_address,uint8 low_address);
#endif 
/* __I2CHARD_H__ */
#endif