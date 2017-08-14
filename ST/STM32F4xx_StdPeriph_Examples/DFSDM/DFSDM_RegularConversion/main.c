/**
  ******************************************************************************
  * @file    DFSDM_RegularConversion/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
  
/** @addtogroup STM32F4xx_Examples
  * @{
  */

/** @addtogroup DFSDM_RegularConversion
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t ConvertedVal = 0;
__IO TestStatus ConversionStatus = FAILED;
/* Private function prototypes -----------------------------------------------*/
void DFSDM_Config(void); /* DFSDM Filter + Channel Configuration */
void DFSDM_GPIO_Config(void); /* DFSDM Pins Configuration */
static TestStatus Valuecmp(uint32_t value1, uint32_t value2);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{ 
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file startup_stm32f412xG.s) before to branch to application main. 
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f4xx.c file
  */
  
  uint32_t ExpectedVal = 0;
  
  /* Calculate expected value */
  ExpectedVal = (int32_t) pow(100, 1);  /* Filter oversampling ratio (100) to the power of filter sinc order (1) */
  ExpectedVal = ExpectedVal*32;         /* previous value multiplied by filter integrator oversampling ratio (32) */
  ExpectedVal = ExpectedVal >> 2;       /* Take into account the channel right bit shift (2) */
  ExpectedVal = ExpectedVal  - 50;      /* Take into account the channel offset (50) */
  
  /* Initialize the Inputs for Audio Recorder : DFSDM + Clocks */
  /* DFSDM Config */
  DFSDM_GPIO_Config();
  
  /* DFSDM config for Conversion : Filter + Channel Initialization */
  DFSDM_Config();
  
  /* start DFSDM conversions */
  /* Connect 3.3v to PB1 DFSDM_DataIn 0 */
  DFSDM_StartSoftwareRegularConversion(DFSDM0);
  
  /* Wait the End of conversion */
  while((DFSDM_GetFlagStatus(DFSDM0, DFSDM_FLAG_REOC) != SET));
  
  /* Get the converted data */
  ConvertedVal = DFSDM_GetRegularConversionData(DFSDM0);
  
  /* Compare the converted value with the expected one */
  ConversionStatus =Valuecmp(ConvertedVal, ExpectedVal);

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Initializes the DFSDM channel GPIO.
  * @param  None
  * @retval None
  */
void DFSDM_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Initstruct;
 
  /* Select DFSDM internal clock source */
  RCC_DFSDM1CLKConfig(RCC_DFSDM1CLKSource_SYS);
  /* Enable DFSDM clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_DFSDM, ENABLE);
  /* Configure PC2 for DFSDM_CKOUT and PB1 for DFSDM_DATIN0 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Common Configuration */
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Initstruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Initstruct.GPIO_Speed = GPIO_High_Speed;
  GPIO_Initstruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  /* DFSDM Clock Output PC2 */
  GPIO_Initstruct.GPIO_Pin = GPIO_Pin_2 ;
  GPIO_Init(GPIOC, &GPIO_Initstruct);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF8_DFSDM1);
  /* DFSDM Data_In_O PB1 */
  GPIO_Initstruct.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_Init(GPIOB, &GPIO_Initstruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF8_DFSDM1);
}

/**
  * @brief  Configure the DFSDM Filter + channel.
  * @param  None
  * @retval None
  */
void DFSDM_Config(void)
{
  DFSDM_TransceiverInitTypeDef DFSDM_TransceiverInitStruct;
  DFSDM_FilterInitTypeDef     DFSDM_FilterInitStruct;
  
  /* DFSDM data acquisition preparation *************************************/
  /* DFSDM_Channel 0 : Configuration */
  DFSDM_TransceiverStructInit(&DFSDM_TransceiverInitStruct);
  DFSDM_TransceiverInitStruct.DFSDM_Input = DFSDM_Input_External;
  DFSDM_TransceiverInitStruct.DFSDM_PackingMode = DFSDM_PackingMode_Standard;
  DFSDM_TransceiverInitStruct.DFSDM_Redirection  = DFSDM_Redirection_Disabled;
  DFSDM_TransceiverInitStruct.DFSDM_Interface =  DFSDM_Interface_SPI_RisingEdge;
  DFSDM_TransceiverInitStruct.DFSDM_ShortCircuitDetector = DFSDM_ShortCircuitDetector_Disable;
  DFSDM_TransceiverInitStruct.DFSDM_CLKAbsenceDetector = DFSDM_CLKAbsenceDetector_Disable;
  DFSDM_TransceiverInitStruct.DFSDM_Offset = 50;  
  DFSDM_TransceiverInitStruct.DFSDM_DataRightShift = 2;
  DFSDM_TransceiverInit(DFSDM1_Channel0, &DFSDM_TransceiverInitStruct);
  
  /* Filter 0 : Configuration */
  DFSDM_FilterInitStruct.DFSDM_SincOrder = DFSDM_SincOrder_Sinc1;
  DFSDM_FilterInitStruct.DFSDM_FilterOversamplingRatio = 100;
  DFSDM_FilterInitStruct.DFSDM_IntegratorOversamplingRatio = 32; 
  DFSDM_FilterInit(DFSDM0, &DFSDM_FilterInitStruct);
    
  /* Connect Channel 1 to Filter 0, Regular mode */
  DFSDM_SelectRegularChannel(DFSDM0, DFSDM_RegularChannel0);
  
  /* Continuous conversion */
  DFSDM_RegularContinuousModeCmd(DFSDM0, ENABLE);
  
  /* Configure CKOUT to deliver a clock output SysClk/5 = 20MHz */
  DFSDM_ConfigClkOutputSource(DFSDM_ClkOutSource_SysClock);
  DFSDM_ConfigClkOutputDivider(5);
  
  /* Enable DFSDM Interface */
  DFSDM_Cmd(ENABLE);
  /* Enable Channel 0 */
  DFSDM_ChannelCmd(DFSDM1_Channel0, ENABLE);
  /* Enable DFSDM Filter 0 */
  DFSDM_FilterCmd(DFSDM0, ENABLE);
}


/**
  * @brief  Return the result of two values's comparison.
  * @param  value1,value2 to compare
  * @retval None
  */
static TestStatus Valuecmp(uint32_t value1, uint32_t value2)
{ 
  if(value1 != value2)
  {
    return FAILED; /* Error */
  }
  else
  {
    return PASSED; /* Success */
  }   
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
