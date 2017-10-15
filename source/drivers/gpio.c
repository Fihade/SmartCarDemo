/*
 * File:	start.c
 * Purpose: gpio initialization
 *
 * Notes:
 */
#include "common.h"
#include "gpio.h"

/******************************************************************************/
/*!
 * \brief   gpio initialization
 * \return  None
 *
 * This function initializes the ports used as gpio
 */
void gpio_init(void)
{
// Set port for GPIO functionality
    JM_0_PCR = (0 | PORT_PCR_MUX(1));
    JM_1_PCR = (0 | PORT_PCR_MUX(1));
    JM_2_PCR = (0 | PORT_PCR_MUX(1));
    JM_3_PCR = (0 | PORT_PCR_MUX(1));
    
    LED_0_PCR = (0 | PORT_PCR_MUX(1));
    LED_1_PCR = (0 | PORT_PCR_MUX(1));
    
    LED_A_PCR = (0 | PORT_PCR_MUX(1));
    LED_B_PCR = (0 | PORT_PCR_MUX(1));
    LED_C_PCR = (0 | PORT_PCR_MUX(1));
    LED_D_PCR = (0 | PORT_PCR_MUX(1));
    ZLG7290_INT_PCR = (0 | PORT_PCR_MUX(1));
    
    for(int i = 8; i < 16; i++)
      {
              PORTC_PCR(i) = 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK;
      }
        
// Set port data direction
    //input
    CLR_BIT(JM_0_DDR, JM_0_PIN);
    CLR_BIT(JM_1_DDR, JM_1_PIN);
    CLR_BIT(JM_2_DDR, JM_2_PIN);
    CLR_BIT(JM_3_DDR, JM_3_PIN);
      
    CLR_BIT(ZLG7290_INT_DDR, ZLG7290_INT_PIN);

    //output
    SET_BIT(LED_0_DDR, LED_0_PIN);
    SET_BIT(LED_1_DDR, LED_1_PIN);
    SET_BIT(LED_A_DDR, LED_A_PIN);
    SET_BIT(LED_B_DDR, LED_B_PIN);
    SET_BIT(LED_C_DDR, LED_C_PIN);
    SET_BIT(LED_D_DDR, LED_D_PIN);

    // ouput initialization
    LED_0_OFF();
    LED_1_OFF();
    LED_A_OFF();
    LED_B_OFF();
    LED_C_OFF();
    LED_D_OFF();

} // gpio_init

/******************************************************************************/

