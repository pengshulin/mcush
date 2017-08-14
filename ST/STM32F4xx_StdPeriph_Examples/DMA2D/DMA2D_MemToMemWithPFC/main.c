/**
  ******************************************************************************
  * @file    DMA2D/DMA2D_MemToMemWithPFC/main.c 
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

/** @addtogroup DMA2D_MemToMemWithPFC
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* DMA2D output address and input for LCD */
uint32_t aBufferResult[18000];

/* Private function prototypes -----------------------------------------------*/
static void DMA2D_Config(void);
static void LCD_Config(void);

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
       
  /* Example description.
     ------------------------------------------
  This example provides a description of how to configure DMA2D periph in 
  Memory_to_Memory with pixel format conversion transfer mode and display the 
  result on LCD.
  In this case two images will be displayed on the LCD, original image before 
  the pixel format conversion and the transferred data after pixel format conversion
  In this example the pixel format conversion is done from ARGB8888 to ARGB4444 
  and you can see on LCD the difference between the two images
   */  
  
  /* DMA2D configuration */
  DMA2D_Config();
  
  /* Start Transfer */
  DMA2D_StartTransfer(); 
  
  /* Wait for CTC Flag activation */
  while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
  {
  }  

  /* Configure LCD */
  LCD_Config();
  
  /* LCD Display */
  LCD_DisplayOn();

  while (1)
  {
  }
}

/**
  * @brief DMA2D configuration.
  * @note  This function Configure tha DMA2D peripheral :
  *        1) Configure the transfer mode : memory to memory W/ pixel format conversion
  *        2) Configure the output color mode as ARGB4444
  *        3) Configure the output memory address at SRAM memory  
  *        4) Configure the data size : 320x120 (pixels)  
  *        5) Configure the input color mode as ARGB8888
  *        6) Configure the input memory address at FLASH memory 
  * @retval
  *  None
  */

static void DMA2D_Config(void)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;
  DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
  
  /* Transfer mode => Memory to memory with pixel format conversion*/
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M_PFC; 
  /* Output color mode */
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_ARGB4444; 
  /* Output Address at SRAM memory */
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = (uint32_t)&aBufferResult;
  /* Initialize the alpha and RGB values */
  DMA2D_InitStruct.DMA2D_OutputGreen = 0;
  DMA2D_InitStruct.DMA2D_OutputBlue = 0;
  DMA2D_InitStruct.DMA2D_OutputRed = 0;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0;
  /* Initialize the output offset */
  DMA2D_InitStruct.DMA2D_OutputOffset = 0;
  /* Number of lines : height */
  DMA2D_InitStruct.DMA2D_NumberOfLine = 120; 
  /* Number of pixel per line : width */
  DMA2D_InitStruct.DMA2D_PixelPerLine = 300;
  
  /* Initialize DMA2D */
  DMA2D_Init(&DMA2D_InitStruct);

  /* Configure default values for foreground */
  DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);
  
  /* Input Address at flash memory */
  DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)&ARGB8888_300x120;
  /* Input color mode */
  DMA2D_FG_InitStruct.DMA2D_FGCM = CM_ARGB8888;
  
  /* Initialize foreground */
  DMA2D_FGConfig(&DMA2D_FG_InitStruct);
}

/**
  * @brief LCD configuration.
  * @note  This function Configure the LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - ARGB8888 as pixel format  
  *           - The frame buffer is located at internal RAM : The output of DMA2D transfer
  *           - The Layer size configuration : 300x120
  *        4) Configure the LTDC Layer 2 :
  *           - ARGB4444 as pixel format  
  *           - The frame buffer is located at internal RAM : The output of DMA2D transfer
  *           - The Layer size configuration : 300x120    
  * @retval
  *  None
  */

static void LCD_Config(void)
{
  /* Initialize the LCD */
  LCD_Init();
  
  /* Initialize the LCD Layers */
  LCD_LayerInit();
  
  /* Set the Foreground as active Layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);

  /* Configure the window size and position */
  LCD_SetDisplayWindow(90, 137, 120, 300);
  
  /* Configure the LCD frame Buffer Address */
  LCD_SetAddress((uint32_t)&aBufferResult);

  /* Configure the LCD Pixel Format */
  LCD_SetPixelFormat(LTDC_Pixelformat_ARGB4444);  
  
  /* Set the Background as active Layer */
  LCD_SetLayer(LCD_BACKGROUND_LAYER); 
  
  /* Configure the window size and position */
  LCD_SetDisplayWindow(90, 15, 120, 300);
  
  /* Configure the LCD frame Buffer Address */
  LCD_SetAddress((uint32_t)&ARGB8888_300x120); 
  
  /* Configure the LCD Pixel Format */
  LCD_SetPixelFormat(LTDC_Pixelformat_ARGB8888);
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
