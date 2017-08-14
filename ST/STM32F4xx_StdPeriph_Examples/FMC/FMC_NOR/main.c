/**
  ******************************************************************************
  * @file    FMC/FMC_NOR/main.c 
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

/** @addtogroup FMC_NOR
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE        ((uint32_t)0x0400)
#define WRITE_READ_ADDR    ((uint16_t)0x8000)
 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Read/Write Buffers */
uint16_t aTxBuffer[BUFFER_SIZE];
uint16_t aRxBuffer[BUFFER_SIZE];

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;
__IO uint32_t uwIDStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;

/* NOR IDs structure */
NOR_IDTypeDef pNOR_ID;

/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLenght, uint16_t uwOffset);

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
  
  /* NOR memory initialization */
  NOR_Init();
  
  /* Initialize the ID structure */
  pNOR_ID.Manufacturer_Code = (uint16_t)0x00;
  pNOR_ID.Device_Code1 = (uint16_t)0x00;
  pNOR_ID.Device_Code2 = (uint16_t)0x00;
  pNOR_ID.Device_Code3 = (uint16_t)0x00;
  
  /* Read the NOR memory ID */
  NOR_ReadID(&pNOR_ID);
  
  /* Test the NOR ID correctness */
  if(pNOR_ID.Manufacturer_Code != (uint16_t)0x0020)
    uwIDStatus++;
  else if(pNOR_ID.Device_Code1 != (uint16_t)0x227E)
    uwIDStatus++;
  else if (pNOR_ID.Device_Code2 != (uint16_t)0x2221)
    uwIDStatus++;
  else if (pNOR_ID.Device_Code3 != (uint16_t)0x2200)
    uwIDStatus++;
    
  /* Return to read mode */
  NOR_ReturnToReadMode();
  
  /* Erase the NOR memory block to write on */
  NOR_EraseBlock(WRITE_READ_ADDR);
  
  /* Fill the buffer to send */
  Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0x2A50);
  
  /* Write data to the NOR memory */
  NOR_WriteBuffer(aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
  
  /* Read data from the NOR memory */
  NOR_ReadBuffer(aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  

  /* Read back the NOR memory and check content correctness */   
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (aRxBuffer[uwIndex] != aTxBuffer[uwIndex])
    {
      uwWriteReadStatus = uwIndex + 1;
    }
  }  
  
  /* Check Data and ID correctness */
  if(uwWriteReadStatus || uwIDStatus)
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
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLenght, uint16_t uwOffset)
{
  uint16_t tmpIndex = 0;

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
