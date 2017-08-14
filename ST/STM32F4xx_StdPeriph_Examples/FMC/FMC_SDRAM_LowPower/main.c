/**
  ******************************************************************************
  * @file    FMC/FMC_SDRAM_LowPower/main.c 
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

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM_LowPower
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE              ((uint32_t)0x0100)
#define WRITE_READ_ADDR          ((uint32_t)0x0800)  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Read/Write Buffers */
uint32_t aTxBuffer[BUFFER_SIZE];
uint32_t aRxBuffer[BUFFER_SIZE];

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;
__IO uint32_t uwSDSR_ModeStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;

/* Global structures */
FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;

/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static void SYSCLKConfig_STOP(void);

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
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
       startup_stm32f429_439xx.s/startup_stm32f401xx.s) before to branch to 
       application main. To reconfigure the default setting of SystemInit() 
       function, refer to system_stm32f4xx.c file
     */       
  
  /* Initialize LEDs mounted on EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2); 
  STM_EVAL_LEDInit(LED3);
  
  /* WAKEUP button (EXTI_Line0) will be used to wakeup the system from STOP mode */
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_EXTI);
  
  /* Initialize the SDRAM memory */
  SDRAM_Init();
  
  /* Fill the buffer to send */
  Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0x250F);
  
  /* Write data to the SDRAM memory */
  SDRAM_WriteBuffer(aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
  
  /* Wait for TAMPER/KEY to be pushed to enter stop mode */
  while(STM_EVAL_PBGetState(BUTTON_TAMPER) != RESET)
  {
  }
  
  /* Program a self-refresh mode command */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_Selfrefresh;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  
  /* Check the bank mode status */
  if(FMC_GetModeStatus(FMC_Bank1_SDRAM) != FMC_SelfRefreshMode_Status)
  {
    uwSDSR_ModeStatus++; 
  } 
  
  /* Put LED3 on to indicate entering to STOP mode */
  STM_EVAL_LEDOn(LED3);  
                        
  /* Request to enter STOP mode */
  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
  
  /* Configure the system clock after wakeup from STOP: enable HSE, PLL and select 
       PLL as system clock source (HSE and PLL are disabled in STOP mode) */
  SYSCLKConfig_STOP();
  
  /* Program a normal mode command */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_normal;
  
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
 
  /* Check the bank mode status */
  if(FMC_GetModeStatus(FMC_Bank1_SDRAM) != FMC_NormalMode_Status)
  {
    uwSDSR_ModeStatus++; 
  }
  
  /* Read back data from the SDRAM memory */
  SDRAM_ReadBuffer(aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  
  
  /* Check the SDRAM memory content's correctness */   
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
    {
      uwWriteReadStatus++;
    }
  }	

  if (uwWriteReadStatus || uwSDSR_ModeStatus)
  {
    /* KO */
    /* Turn on LD2 */
    STM_EVAL_LEDOn(LED2);     
  }
  else
  { 
    /* OK */
    /* Turn on LD1 */
    STM_EVAL_LEDOn(LED1);
  }

  while (1)
  {
  } 
  
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* Wait till HSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {}
  
  /* Enable PLL */
  RCC_PLLCmd(ENABLE);
  
  /* Wait till PLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}
  
  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {}
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  BufferLenght: size of the buffer to fill
  * @param  Offset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
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
