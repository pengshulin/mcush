/**
  ******************************************************************************
  * @file    SAI/SAI_Audio/main.c 
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
#include "audio_sample.h"

/** @addtogroup SAI_Audio
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Audio file size and start address are defined here since the audio file is 
   stored in Flash memory as a constant table of 16-bit data */
#define AUDIO_FILE_SIZE         1513368
#define AUDIO_START_ADDRESS     58 /* Offset relative to audio file header size */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
static RCC_ClocksTypeDef RCC_Clocks;
static uint32_t uwVolume = 70;

/* Private function prototypes -----------------------------------------------*/
static void PLLSAI_Config(void);

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
       files (startup_stm32f429_439xx.s) before to branch to application main.
     */ 
  
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Initialize LEDs mounted on EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);

  /* KEY button for Volume High */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* WAKEUP button for Volume Low */  
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_GPIO); 
  
  /* Configure PLLSAI */
  PLLSAI_Config();
  
  /* Initialize the Audio codec and all related peripherals (SAI, I2C, IOs...) */  
  if (EVAL_AUDIO_Init(OUTPUT_DEVICE_BOTH, uwVolume, SAI_AudioFreq_48k) == 0)
  {
    /* AUDIO CODEC OK */
    STM_EVAL_LEDOn(LED1);
  }
  else
  {
    /* AUDIO CODEC FAIL */ 
    STM_EVAL_LEDOn(LED2);
  }
  /* Insert 50 ms delay */
  Delay(5);
  
  /* Play audio sample stored in internal FLASH */
  EVAL_AUDIO_Play((uint16_t*)(AUDIO_SAMPLE + AUDIO_START_ADDRESS), (AUDIO_FILE_SIZE - AUDIO_START_ADDRESS));
 
  /* Infinite loop */
  while(1)
  {
    /* Check on the Volume high button */
    if (STM_EVAL_PBGetState(BUTTON_TAMPER) != Bit_SET)
    {
      /* Wait to avoid rebound */
      while (STM_EVAL_PBGetState(BUTTON_TAMPER) != Bit_SET);
      
      /* Increase volume by 5% */
      if (uwVolume < 95)
      {
        uwVolume += 5;
      } 
      else
      {
        uwVolume = 100; 
      }
      /* Apply the new volume to the codec */
      EVAL_AUDIO_VolumeCtl(uwVolume);
    }
    /* Check on the Volume high button */
    if (STM_EVAL_PBGetState(BUTTON_WAKEUP) == Bit_SET)
    {
      /* Wait to avoid rebound */
      while (STM_EVAL_PBGetState(BUTTON_WAKEUP) == Bit_SET);
      
      /* Decrease volume by 5% */
      if (uwVolume > 5)
      {
        uwVolume -= 5;
      } 
      else
      {
        uwVolume = 0; 
       }
      /* Apply the new volume to the codec */
      EVAL_AUDIO_VolumeCtl(uwVolume);
    }
    
  } 
}

/**
  * @brief  Configure PLLSAI.
  * @param  None
  * @retval None
  */
void PLLSAI_Config(void)
{ 
  /* Configure PLLSAI prescalers */
  /* PLLSAI_VCO : VCO_429M */
  /* SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz */
  RCC_PLLSAIConfig(429, 2, 4);
  
  /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */  
  RCC_SAIPLLSAIClkDivConfig(19);
  
  /* Configure PLLI2S prescalers */
  /* PLLI2S_VCO : VCO_344M */
  /* SAI_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 344/7 = 49.142 Mhz */
  RCC_PLLI2SConfig(344, 7, 4);
  
  /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ = 49.142/1 = 49.142 Mhz */  
  RCC_SAIPLLI2SClkDivConfig(1);
  
  /* Configure Clock source for SAI Block A */
  RCC_SAIBlockACLKConfig(RCC_SAIACLKSource_PLLSAI);
  
  /* Configure Clock source for SAI Block B */
  RCC_SAIBlockBCLKConfig(RCC_SAIBCLKSource_PLLI2S);
  
  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  
  /* Wait till PLLSAI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET) 
  {
  }
  
  /* Enable PLLI2S Clock */
  RCC_PLLI2SCmd(ENABLE);
  
  /* Wait till PLLI2S is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET) 
  {
  }
}

/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
  /* Calculate the remaining audio data in the file and the new size 
     for the DMA transfer. If the Audio files size is less than the DMA max 
     data transfer size, so there is no calculation to be done, just restart 
     from the beginning of the file ... */
  /* Check if the end of file has been reached */

#ifdef AUDIO_MAL_MODE_NORMAL  

  /* Replay from the beginning */
  EVAL_AUDIO_Play((uint16_t*)(AUDIO_SAMPLE + AUDIO_START_ADDRESS), (AUDIO_FILE_SIZE - AUDIO_START_ADDRESS));

#endif /* AUDIO_MAL_MODE_NORMAL */
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  uwTimingDelay = nTime;
  
  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
