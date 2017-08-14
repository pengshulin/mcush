/**
  ******************************************************************************
  * @file    LPTIM/LPTIM_PulseCounter/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Program Body
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

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup LPTIM_PulseCounter
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* LPTIM handle declaration */
LPTIM_InitTypeDef             LPTIM_InitStructure;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void LPTIM_Config(void);
void LED_Config(void);
void MCO2_Config(void);
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
       files (startup_stm32f410xx.s) before to branch to application main.
     */   
  
  /* LPTIM GPIO & NVIC & EXTI Configuration */
  LPTIM_Config();
  /* LED2 Green (PA5) GPIO Configuration */
  LED_Config();
  /* OutPut SysCLK divided by 5 on MCO2 (PC9) */
  MCO2_Config();
  
  /* ### - 1 - Re-target the LSI to Clock the LPTIM Clock Source #############*/
  /* Select the LSI clock as LPTIM peripheral clock */
  RCC_LSICmd(ENABLE);
  
  /* Wait until LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Disable PLL */
  RCC_PLLCmd(DISABLE);
  
  /* Select LSI as LPTIM Clock Source */
  RCC_LPTIM1ClockSourceConfig(RCC_LPTIM1CLKSOURCE_LSI);
  /* Check if LSI is selected for LPTIM clock source */
  if((RCC->DCKCFGR2 & RCC_LPTIM1CLKSOURCE_LSI) != RCC_LPTIM1CLKSOURCE_LSI)
  {
    while(1);
  }
  
  /* ### - 2 - Initialize the LPTIM peripheral ############################## */
  /*
   *  Instance        = LPTIM1
   *  Clock Source    = APB or LowPowerOSCillator (in this example LSI is
   *                    already selected from the RCC stage)
   *  Counter source  = External Clock (connect Input at PB5) 1kHz
   *  Clock prescaler = 1 (No division)
   *  Counter Trigger = Software trigger
   *  Output Polarity = High
   *  Update mode     = Immediate (Registers are immediately updated after any
   *                    write access) 
   */
  /* Deinitialze LPTIM peripheral */
  LPTIM_DeInit(LPTIM1);
  /* Initialize the LPTIM peripheral */
  LPTIM_InitStructure.LPTIM_ClockSource    = LPTIM_ClockSource_APBClock_LPosc; 
  LPTIM_InitStructure.LPTIM_OutputPolarity = LPTIM_OutputPolarity_High;
  LPTIM_InitStructure.LPTIM_Prescaler      = LPTIM_Prescaler_DIV1; 
  LPTIM_InitStructure.LPTIM_Waveform       = LPTIM_Waveform_PWM_OnePulse;
  LPTIM_Init(LPTIM1, &LPTIM_InitStructure); 
  LPTIM_ConfigUpdate(LPTIM1, LPTIM_Update_Immediate);
  /* Select External Clock as LPTIM clock source counter */
  LPTIM_SelectClockSource(LPTIM1,LPTIM_ClockSource_ULPTIM);
  
  /* ### - 3 - Counter Mode in Interrupt mode configuration ##################*/
  /* Select software start Trigger */
  LPTIM_SelectSoftwareStart(LPTIM1);
  /* Enable Counter Mode */
  LPTIM_SelectCounterMode(LPTIM1, ENABLE);
  /* Enables AutoReload Interrupts */
  LPTIM_ITConfig(LPTIM1, LPTIM_IT_ARROK, ENABLE);
  LPTIM_ITConfig(LPTIM1, LPTIM_IT_ARRM, ENABLE);
  /* Enable LPTIMER */  
  LPTIM_Cmd(LPTIM1, ENABLE);
  /* Set Autoreload Value */
  LPTIM_SetAutoreloadValue(LPTIM1, 1000); /* Period 1s for 1kHz Input */
  /* Starts the LPTIMER in continuous mode */
  LPTIM_SelectOperatingMode(LPTIM1, LPTIM_Mode_Continuous);
  
  /* Enter Stop Mode */  
  PWR_EnterSTOPMode(PWR_LowPowerRegulator_ON, PWR_STOPEntry_WFI);

  /* Infinite Loop */
  while (1)
  {
  }
}

/**
  * @brief  LPTIM GPIO,NVIC Configuration and EXTI LINE 23 configuration
  * @param  None
  * @retval None
  */
void LPTIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Enable LPTIM Peripheral */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LPTIM1, ENABLE);
  /* Enable GPIO for LPTIM Pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
  
  /* Configure Pin PB5 for Input 1 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_LPTIM);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Configure External Trigger Pins (PC3,PB6) */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_LPTIM);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_LPTIM);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Enable the LPTIM global Interrupt */
  /* Configure the Priority Group to 1 bit */                
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel = LPTIM1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* EXTI Line 23 Configuration */
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = EXTI_Line23;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStructure);
}

/**
  * @brief  LED Green GPIO Configuration
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO for LED Pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
  /* Configure LED2 (PA5) as Output */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
/**
  * @brief  Output System Clock on MCO2 
  * @param  None
  * @retval None
  */
void MCO2_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Config MCO2 PC9 to Alternate function */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  
  /* QSPI CLK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_5);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
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
