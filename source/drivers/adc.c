/*
* File:	adc.c
* Purpose:	collect ad simple value
*
* Notes:		
*/
#include "common.h"
#include "adc.h"

//============================================================================
//函数名称：adc_init
//函数返回：
//参数说明：
//功能概要：AD初始化
//============================================================================
void ADC_Init(void)
{
  
#if(0==ADC_CHANNEL)
  SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );
  //#elif(1==SPI_CHANNEL)
#elif(1==ADC_CHANNEL)
  SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );
#elif(2==ADC_CHANNEL)
  SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );
  SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );
#else
#error "Definition of ADC_CHANNEL ERROR ! "
#endif
  
}

//============================================================================
//函数名称：ad_once
//函数返回：16位无符号的AD值 
//参数说明：MoudelNumber：模块号
//               Channel：通道号
//              accuracy：精度
//               ab: 0为a通道，1为b通道
//功能概要：采集一次一路模拟量的AD值    
//============================================================================
uint16 ADC_Once(int MoudelNumber,int Channel,uint8 accuracy,uint8 ab)//采集某路模拟量的AD值
{
  uint16 result = 0;
  uint8 ADCCfg1Mode = 0;
  ADC_MemMapPtr ADCMoudel;//保存ADC模块地址指针
  
  switch(accuracy)
  {
  case 8:
    ADCCfg1Mode = 0x00;
    break;
  case 12:
    ADCCfg1Mode = 0x01;
    break;
  case 10:
    ADCCfg1Mode = 0x02;
    break;
  case 16:
    ADCCfg1Mode = 0x03;
    break;
  default:
    ADCCfg1Mode = 0x00;
  }
  
  
  if(MoudelNumber==0)//选择ADC模块0
  {
    ADCMoudel = ADC0_BASE_PTR;
  }
  else               //选择ADC模块1
  {
    ADCMoudel = ADC1_BASE_PTR;
  }
  
  //配置正常电源模式，总线时钟，总线时钟4分频，长采样时间使能，设置精度
  ADC_CFG1_REG(ADCMoudel) = ADLPC_NORMAL
    | ADC_CFG1_ADIV(ADIV_4)
      | ADLSMP_LONG
        | ADC_CFG1_MODE(ADCCfg1Mode)
          | ADC_CFG1_ADICLK(ADICLK_BUS);
  
  //配置禁止异步时钟使能输出，ADxxat通道选择，高速配置，长采样时间   
  if(ab==0)
  {
    ADC_CFG2_REG(ADCMoudel)  =    MUXSEL_ADCA
      | ADACKEN_DISABLED
        | ADHSC_HISPEED
          | ADC_CFG2_ADLSTS(ADLSTS_2) ;
  }
  else
  {
    ADC_CFG2_REG(ADCMoudel)  =    MUXSEL_ADCB
      | ADACKEN_DISABLED
        | ADHSC_HISPEED
          | ADC_CFG2_ADLSTS(ADLSTS_2) ;
  }
  
  //设置通道号
  //ADC_SC1_REG(ADCMoudel,A) = AIEN_ON |DIFF_DIFFERENTIAL | ADC_SC1_ADCH(Channel);
  ADC_SC1_REG(ADCMoudel,A) = AIEN_ON |DIFF_SINGLE | ADC_SC1_ADCH(Channel);
  //等待转换完成
  while (( ADC_SC1_REG(ADCMoudel,A) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK)
  {
    
  }
  //读取转化结果
  result = ADC_R_REG(ADCMoudel,A);       
  //清ADC转换完成标志
  ADC_SC1_REG(ADCMoudel,A) &= ~ADC_SC1_COCO_MASK;
  
  return result;
}
/******************************************************************************/
/*!
 * \brief   adc get
 * \return  None
 *
 *
 */
uint16 adc_get(void)
{
    // start the conversion
    // software trigger and hardware trigger. When software trigger is selected,
    // a conversion is initiated following a write to SC1n.
    ADC_SC1_REG(ADC_PTR,ADC_AB) = ADC_SC1_ADCH(23);
    
    // wait
    while(!(ADC_SC1_REG(ADC_PTR,ADC_AB) & ADC_SC1_COCO_MASK));
    
    // return the result of conversion
    return (uint16)ADC_R_REG(ADC_PTR, ADC_AB);
}
/******************************************************************************/

uint16 Get_AD_Channel(int16 i)
{
  uint16 result=0;
  switch(i)
  { 
    case 0:
      result = ADC_Once(1,4,16,0); //VOUT_2 PTE0
      break;
    case 1:
      result = ADC_Once(0,5,16,1); //VOUT_1 PTD1
      break;
    case 2:
      result = ADC_Once(1,6,16,0); //xOUT PTE2
      break;
    case 3:
      result = ADC_Once(1,5,16,0); //yOUT PTE1
      break;
    case 4:
      result = ADC_Once(1,7,16,0); // zOUT PTE3
      break;
    case 5:
      //result = ADC_Once(1,7,16,0); //ad_gyro_2
      break;
    case 6:
      //result = ADC_Once(1,18,16,0); //ad_curr_b 18  VOUT2
      break;
    case 7:
      //result = ADC_Once(0,18,16,0); //ad_curr_b 18  VOUT1
      break;
    case 8:
      //result = ADC_Once(0,23,16,0); //ad_curr_a 23
    default:
      result = 0;
      break;
      
  }
  return result;
}
