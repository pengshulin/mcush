/**
  ******************************************************************************
  * @file    LTDC/LTDC_Display_2Layers/main.c 
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

/** @addtogroup LTDC_Display_2Layers
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void LCD_Config(void); 
static void LCD_AF_GPIOConfig(void);

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

  /* Configure LCD : Configure 2 layers w/ Blending and CLUT loading for layer 1 */
  LCD_Config(); 
    
  /* Enable CLUT for Layer 1 */
  LTDC_CLUTCmd(LTDC_Layer1, ENABLE);

  /* Enable Layer 1 */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
  
  /* Enable Layer 2 */
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
  
  /* Reload LTDC configuration  */
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable The LCD */
  LTDC_Cmd(ENABLE);

  while (1)
  {
  }
}

/**
  * @brief LCD Configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - indirect color (L8) as pixel format  
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 320x240
  *        4) Configure the LTDC Layer 2 :
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 320x240  
  *        5) Load 256 colors in CLUT address for Layer 1                       
  * @retval
  *  None
  */
static void LCD_Config(void)
{
  LTDC_InitTypeDef       LTDC_InitStruct;
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;
  LTDC_CLUT_InitTypeDef  LTDC_CLUT_InitStruct;

  uint32_t uwCounter = 0;
/* Enable Pixel Clock --------------------------------------------------------*/
  /* Configure PLLSAI prescalers for LCD */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/3 = 64 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64/8 = 8 Mhz */
  RCC_PLLSAIConfig(192, 7, 3);
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);
  
  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  /* Wait for PLLSAI activation */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }
  
/* Enable the LTDC Clock -----------------------------------------------------*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
  
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_AF_GPIOConfig();
  
/* Initialize the LCD --------------------------------------------------------*/

  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low*/
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* Initialize the vertical synchronization polarity as active low */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* Initialize the data enable polarity as active low */ 
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* Initialize the pixel clock polarity as input pixel clock */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* Timing configuration */
  /* Horizontal synchronization width = Hsync - 1 */     
  LTDC_InitStruct.LTDC_HorizontalSync = 40;
  /* Vertical synchronization height = Vsync - 1 */
  LTDC_InitStruct.LTDC_VerticalSync = 9;
  /* Accumulated horizontal back porch = Hsync + HBP - 1 */
  LTDC_InitStruct.LTDC_AccumulatedHBP = 42; 
  /* Accumulated vertical back porch = Vsync + VBP - 1 */
  LTDC_InitStruct.LTDC_AccumulatedVBP = 11;  
  /* Accumulated active width = Hsync + HBP + Active Width - 1 */  
  LTDC_InitStruct.LTDC_AccumulatedActiveW = 522;
  /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = 283;
  /* Total width = Hsync + HBP + Active Width + HFP - 1 */
  LTDC_InitStruct.LTDC_TotalWidth = 524; 
  /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
  LTDC_InitStruct.LTDC_TotalHeigh = 285;
 
  /* Configure R,G,B component values for LCD background color */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 
  
  /* Initialize LTDC */          
  LTDC_Init(&LTDC_InitStruct);

/* LCD initializing end ------------------------------------------------------*/

/* Layer1 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */ 
  /* In this case only 320x240 window of the active display area is used 
     to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 43 
     Vertical start   = vertical synchronization + vertical back porch     = 12
     Horizontal stop = Horizontal start + window width -1 = 43 + 320 -1 
     Vertical stop   = Vertical start + window height -1  = 12 + 240 -1      */ 
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 43;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (320 + 43 - 1); 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 12;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (240 + 12 - 1);
  
  /* Pixel Format configuration : indirect color (L8) */           
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_L8;
  
  /* Alpha constant configuration */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  
  /* Blending Factors */    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
   
  /* Start Address configuration : frame buffer is located at FLASH memory */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)&L8_320x240;
  
  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3 
     Active high width         = 320 
     number of bytes per pixel = 1    (pixel_format : L8) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((320 * 1) + 3);
  
  /*  the pitch is the increment from the start of one line of pixels to the 
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     
  */
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (320 * 1);  
  
  /* Configure the number of lines */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 240;
  
  /* Initialize the Layer1 */ 
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct); 

/* Layer1 Configuration end --------------------------------------------------*/

/* Layer2 Configuration ------------------------------------------------------*/
 
  /* Windowing configuration */ 
  /* In this case only 320x240 window of the active display area is used 
     to display a picture then :
     Horizontal start = horizontal synchronization + offset_x + Horizontal back porch = 43 
     Vertical start   = vertical synchronization + offset_y + vertical back porch     = 12
     Horizontal stop = Horizontal start + offset_x + window width -1 = 43 + 320 -1 
     Vertical stop   = Vertical start + offset_y + window height -1  = 12 + 240 -1      */  
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 160 + 43;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (320 + 160 + 43 - 1); 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 32 + 12;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (32 + 240 + 12 - 1);
  
  /* Pixel Format configuration */           
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  
  /* Alpha constant configuration : The constant alpha for layer 2 is decreased 
    to see the layer 1 in the intersection zone*/
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 200;
  
  /* Default Color configuration (configure A,R,G,B component values) */           
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  
  /* blending Factors */    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* Configure Input Address : frame buffer is located at FLASH memory */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)&RGB565_320x240;
  
  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3 
     Active high width         = 320 
     number of bytes per pixel = 2    (pixel_format : RGB565) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((320 * 2) + 3); 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (320 * 2);
  
  /*  the pitch is the increment from the start of one line of pixels to the 
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 240; 
  
  /* Initialize the Layer 2 */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

/* Layer2 Configuration end --------------------------------------------------*/
   
/* CLUT loading --------------------------------------------------------------*/

  /* Load 256 colors in CLUT address for Layer 1 */
  for(uwCounter = 0; (uwCounter < 256); uwCounter++)
  {
    LTDC_CLUT_InitStruct.LTDC_CLUTAdress = uwCounter;
    LTDC_CLUT_InitStruct.LTDC_BlueValue = (L8_320x240_CLUT[uwCounter] & 0xFF);
    LTDC_CLUT_InitStruct.LTDC_GreenValue = (L8_320x240_CLUT[uwCounter] & 0xFF00) >> 8;
    LTDC_CLUT_InitStruct.LTDC_RedValue = (L8_320x240_CLUT[uwCounter] & 0xFF0000) >> 16;
    LTDC_CLUTInit(LTDC_Layer1, &LTDC_CLUT_InitStruct);   
  } 
/* CLUT loading end ----------------------------------------------------------*/
  
}  

/**
  * @brief GPIO configuration for LTDC peripheral.
  * @retval
  *  None
  */
static void LCD_AF_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Enable GPIOI, GPIOJ, GPIOG, GPIOF, GPIOH AHB Clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ | \
                         RCC_AHB1Periph_GPIOK,  ENABLE);

/* GPIOs Configuration */
/*
 +------------------------+-----------------------+----------------------------+
 +                       LCD pins assignment                                   +
 +------------------------+-----------------------+----------------------------+
 |  LCD_TFT R0 <-> PI.15  |  LCD_TFT G0 <-> PJ.07 |  LCD_TFT B0 <-> PJ.12      |
 |  LCD_TFT R1 <-> PJ.00  |  LCD_TFT G1 <-> PJ.08 |  LCD_TFT B1 <-> PJ.13      |
 |  LCD_TFT R2 <-> PJ.01  |  LCD_TFT G2 <-> PJ.09 |  LCD_TFT B2 <-> PJ.14      |
 |  LCD_TFT R3 <-> PJ.02  |  LCD_TFT G3 <-> PJ.10 |  LCD_TFT B3 <-> PJ.15      |
 |  LCD_TFT R4 <-> PJ.03  |  LCD_TFT G4 <-> PJ.11 |  LCD_TFT B4 <-> PK.03      |
 |  LCD_TFT R5 <-> PJ.04  |  LCD_TFT G5 <-> PK.00 |  LCD_TFT B5 <-> PK.04      |
 |  LCD_TFT R6 <-> PJ.05  |  LCD_TFT G6 <-> PK.01 |  LCD_TFT B6 <-> PK.05      |
 |  LCD_TFT R7 <-> PJ.06  |  LCD_TFT G7 <-> PK.02 |  LCD_TFT B7 <-> PK.06      |
 -------------------------------------------------------------------------------
          |  LCD_TFT HSYNC <-> PI.12  | LCDTFT VSYNC <->  PI.13 |
          |  LCD_TFT CLK   <-> PI.14  | LCD_TFT DE   <->  PK.07 |
           -----------------------------------------------------                     
*/

 /* GPIOI configuration */
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource12, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource13, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource14, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource15, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
                             
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOI, &GPIO_InitStruct);
  
 /* GPIOJ configuration */  
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource0, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource1, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource2, GPIO_AF_LTDC); 
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource4, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource5, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource8, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource9, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource12, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource13, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource14, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOJ, GPIO_PinSource15, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | \
                             GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | \
                             GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | \
                             GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  
  GPIO_Init(GPIOJ, &GPIO_InitStruct);

 /* GPIOI configuration */
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource0, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource1, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource2, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource4, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource5, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOK, GPIO_PinSource7, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | \
                             GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
                             
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOK, &GPIO_InitStruct);  
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
