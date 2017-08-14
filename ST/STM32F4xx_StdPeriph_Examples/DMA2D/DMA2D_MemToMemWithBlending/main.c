/**
  ******************************************************************************
  * @file    DMA2D/DMA2D_MemToMemWithBlending/main.c 
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

/** @addtogroup DMA2D_MemToMemWithBlending
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  
uint32_t aBlendedImage[19550]; 
  
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
    
  /* Configure DMA2D : Configure foreground and background */
  DMA2D_Config();
  
  /* Start Transfer */ 
  DMA2D_StartTransfer();
  
  /* Wait for TC Flag activation */
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
  *        1) Configure the Transfer mode as memory to memory with blending.
  *        2) Configure the output color mode as RGB565 pixel format.
  *        3) Configure the foreground
  *          - first image loaded from FLASH memory
  *          - constant alpha value (decreased to see the background)
  *          - color mode as RGB565 pixel format
  *        4) Configure the background
  *          - second image loaded from FLASH memory
  *          - color mode as RGB565 pixel format
  * @retval
  *  None
  */

static void DMA2D_Config(void)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;
  DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;  
  DMA2D_BG_InitTypeDef   DMA2D_BG_InitStruct;

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE); 

  /* DMA2D DeInitialization */
  DMA2D_DeInit();
  
  /* Configure transfer mode : memory to memory with blending */
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M_BLEND; 
  
  /* Configure color mode : RGB565 pixel format */
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;  
   
  /* Configure output Address : output frame buffer is located at SRAM */                
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = (uint32_t)&aBlendedImage;
  
  /* Configure the alpha and RGB values */
  DMA2D_InitStruct.DMA2D_OutputGreen = 0;
  DMA2D_InitStruct.DMA2D_OutputBlue = 0;
  DMA2D_InitStruct.DMA2D_OutputRed = 0;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0;

  /* Configure the output offset */
  DMA2D_InitStruct.DMA2D_OutputOffset = 0;  
             
  /* Configure number of lines : height */
  DMA2D_InitStruct.DMA2D_NumberOfLine = 130; 
  
  /* Configure number of pixel per line : width */
  DMA2D_InitStruct.DMA2D_PixelPerLine = 240;
  
  /* DMA2D Initialization */
  DMA2D_Init(&DMA2D_InitStruct);

  /* Configure default values for foreground */
  DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);
  
  /* Configure foreground memory address : first input memory address at FLASH memory */
  DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)&RGB565_240x130_1;
  
  /* Configure foreground color mode */
  DMA2D_FG_InitStruct.DMA2D_FGCM = CM_RGB565;
  
  /* Configure foreground alpha value */
  DMA2D_FG_InitStruct.DMA2D_FGPFC_ALPHA_MODE = REPLACE_ALPHA_VALUE;
  
  /* Configure foreground alpha mode : The constant alpha for foreground is decreased 
    to see the background */
  DMA2D_FG_InitStruct.DMA2D_FGPFC_ALPHA_VALUE = 0x75;

  /* Foreground Initialization */
  DMA2D_FGConfig(&DMA2D_FG_InitStruct);

  /* Configure default values for background */
  DMA2D_BG_StructInit(&DMA2D_BG_InitStruct);
  
  /* Configure background memory address : second input memory address at FLASH memory */
  DMA2D_BG_InitStruct.DMA2D_BGMA = (uint32_t)&RGB565_240x130_2;
  
  /* Configure background color mode */
  DMA2D_BG_InitStruct.DMA2D_BGCM = CM_RGB565;
  
  /* Configure background alpha mode */
  DMA2D_BG_InitStruct.DMA2D_BGPFC_ALPHA_MODE = NO_MODIF_ALPHA_VALUE;
  
  /* Configure background alpha value */
  DMA2D_BG_InitStruct.DMA2D_BGPFC_ALPHA_VALUE = 255;
  
  /* Background Initialization */
  DMA2D_BGConfig(&DMA2D_BG_InitStruct);

}

/**
  * @brief LCD configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - RGB565 as pixel format  
  *           - The frame buffer is located at internal RAM : The output of DMA2D transfer
  *           - The Layer size configuration : 240x130
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

  LCD_SetTransparency(0);
  
  /* Set the Background as active Layer */
  LCD_SetLayer(LCD_BACKGROUND_LAYER); 
  
  /* Configure the window size and position */
  LCD_SetDisplayWindow(120, 70, 130, 240);
  
  /* Configure the LCD frame Buffer Address */
  LCD_SetAddress((uint32_t)&aBlendedImage);  
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
