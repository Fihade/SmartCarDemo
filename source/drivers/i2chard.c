/*
 * File:	i2c.c
 * Purpose:	Inter-Integrated Circuit
 *
 * Notes:		
 */
#if(1)
#include "common.h"
#include "i2chard.h"
/******************************************************************************/
/*!
 * I2C Initialization
 * Set Baud Rate and turn on I2C0
 */
void i2c_init_add(void)
{
    // open the clock gate
#if(0==I2C_CHANNEL)
    SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
#elif(1==I2C_CHANNEL)
    SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;
#else
#error "Definition I2C_CHANNEL ERROR ! "
#endif

    /* configure the port for I2C function */
    I2C_SCL_PCR = PORT_PCR_MUX(I2C_SCL_ALTn);
    I2C_SDA_PCR = PORT_PCR_MUX(I2C_SDA_ALTn);
    
    I2C_SDA_PCR |= PORT_PCR_ODE_MASK;

    /* set MULT and ICR */
    I2C_F_REG(I2C_CHAN_PTR) = I2C_F_MULT(0x01) | I2C_F_ICR(I2C_BAUD_ICR_ADD);

    /* enable I2C */
    I2C_C1_REG(I2C_CHAN_PTR) = I2C_C1_IICEN_MASK;
}

void i2c_Wait_add()
{
  	int cnt = 0;
	while(  (I2C_S_REG(I2C_CHAN_PTR) & I2C_S_IICIF_MASK)  ==  0)
	{
		cnt ++;
		Delay_T3_uS(5);
		if(cnt >1000) return;
	}
	I2C_S_REG(I2C_CHAN_PTR) |= I2C_S_IICIF_MASK;
	return;
}

void i2c_write_register_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data)
{
    /* start */
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(DEVICE_ADDRESS_add);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(register_address);
    i2c_Wait_add();
    /* Write data */
    i2c_write_byte_add(data);
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
}

void i2c_write_reg2byte_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data1, uint8 data2)
{
    /* start */
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(DEVICE_ADDRESS_add & 0xfe);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(register_address);
    i2c_Wait_add();
    /* Write data */
    i2c_write_byte_add(data1);
    i2c_Wait_add();
    /* Write data */
    i2c_write_byte_add(data2);
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
}

/******************************************************************************/
void i2c_write_regNsame_add(uint8 DEVICE_ADDRESS_add, uint8 register_address, uint8 data, uint8 n)
{
    /* start */
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(DEVICE_ADDRESS_add & 0xfe);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(register_address);
    i2c_Wait_add();
    /* Write data */
    while(n--)
    {
        i2c_write_byte_add(data);
        i2c_Wait_add();
    }
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
}

/******************************************************************************/
uint8 i2c_read_register_add(uint8 DEVICE_ADDRESS_add, uint8 register_address)
{
    uint8 rec_data;
    /* start */
    i2c_Start_add();
    /* Send Slave Address */
    i2c_write_byte_add(DEVICE_ADDRESS_add & 0xfe);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(register_address);
    i2c_Wait_add();
    /* Do a repeated start */
    i2c_RepeatedStart_add();
    /* Send Slave Address */
    i2c_write_byte_add(DEVICE_ADDRESS_add | 0x01);
    i2c_Wait_add();
    /* Put in Rx Mode */
    i2c_EnterRxMode_add();
    /* Turn off ACK since this is second to last byte being read*/
    i2c_DisableAck_add();
    /* Dummy read */
    i2c_read_byte_add(rec_data); 
    i2c_Wait_add();
    /* Send stop since about to read last byte */
    i2c_Stop_add();
    /* Read byte */
    i2c_read_byte_add(rec_data);
    Delay_T3_uS(50);
    
    return rec_data;
}


/******************************************************************************/
void EEPROM_write_register_add(uint8 DEVICE_ADDRESS_add, uint8 high_address, uint8 low_address, uint8 data)
{
    /* start */
    i2c_Start_add();
    /* send data to slave */
    i2c_write_byte_add(DEVICE_ADDRESS_add);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(high_address);
    i2c_Wait_add();
    i2c_write_byte_add(low_address);
    i2c_Wait_add();
    /* Write data */
    i2c_write_byte_add(data);
    i2c_Wait_add();
    /* stop */
    i2c_Stop_add();
    Delay_T3_uS(50);
}


uint8 EEPROM_read_register_add(uint8 DEVICE_ADDRESS_add, uint8 high_address,uint8 low_address)
{
    uint8 rec_data;
    /* start */
    i2c_Start_add();
    /* Send Slave Address */
    i2c_write_byte_add(DEVICE_ADDRESS_add & 0xfe);
    i2c_Wait_add();
    /* Write Register Address */
    i2c_write_byte_add(high_address);
    i2c_Wait_add();
    i2c_write_byte_add(low_address);
    i2c_Wait_add();
    /* Do a repeated start */
    i2c_RepeatedStart_add();
    /* Send Slave Address */
    i2c_write_byte_add(DEVICE_ADDRESS_add | 0x01);
    i2c_Wait_add();
    /* Put in Rx Mode */
    i2c_EnterRxMode_add();
    /* Turn off ACK since this is second to last byte being read*/
    i2c_DisableAck_add();
    /* Dummy read */
    i2c_read_byte_add(rec_data); 
    i2c_Wait_add();
    /* Send stop since about to read last byte */
    i2c_Stop_add();
    /* Read byte */
    i2c_read_byte_add(rec_data);
    Delay_T3_uS(50);
    
    return rec_data;
}
#endif



