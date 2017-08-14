/**
  ******************************************************************************
  * @file    LTDC/LTDC_ColorKeying/main.c 
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

/** @addtogroup LTDC_ColorKeying
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PRESSED_FIRST    0x00
#define PRESSED_SECOND   0x01
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t ubPressedButton = PRESSED_FIRST;

LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;
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
  
  /* TAMPER button will be used */
  STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE_GPIO);
  
  /* LCD Configuration */
  LCD_Config();

  /* Enable Layer1 */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
   
  /* Reload configuration of Layer1 */
  LTDC_ReloadConfig(LTDC_IMReload);
 
  /* Enable The LCD */
  LTDC_Cmd(ENABLE);

  while (1)
  {
    /* Wait for tamper button is pressed */
    while (STM_EVAL_PBGetState(BUTTON_TAMPER) != Bit_RESET)
    {
    }

    /* Wait for tamper button is released */
    while (STM_EVAL_PBGetState(BUTTON_TAMPER) != Bit_SET)
    {
    }
  
    if(ubPressedButton == PRESSED_FIRST)
    {
      /* Color Keying enable */
      LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE);
      ubPressedButton = PRESSED_SECOND;  
    }
    else
    {
      /* Color Keying disable */
      LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE);
      ubPressedButton = PRESSED_FIRST; 
    }  
  }
}

/**
  * @brief LCD configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 480x272
  *        4) Configure the Color Keying for Layer 1.    
  * @retval
  *  None
  */

static void LCD_Config(void)
{
  LTDC_InitTypeDef               LTDC_InitStruct;
  LTDC_Layer_InitTypeDef         LTDC_Layer_InitStruct;

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
  
/* LTDC Initialization -------------------------------------------------------*/

  /* Initialize the horizontal synchronization polarity as active low*/
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* Initialize the vertical synchronization polarity as active low */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* Initialize the data enable polarity as active low */ 
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* Initialize the pixel clock polarity as input pixel clock */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* Timing configuration */
  /* Configure horizontal synchronization width */     
  LTDC_InitStruct.LTDC_HorizontalSync = 40;
  /* Configure vertical synchronization height */
  LTDC_InitStruct.LTDC_VerticalSync = 9;
  /* Configure accumulated horizontal back porch */
  LTDC_InitStruct.LTDC_AccumulatedHBP = 42; 
  /* Configure accumulated vertical back porch */
  LTDC_InitStruct.LTDC_AccumulatedVBP = 11;  
  /* Configure accumulated active width */  
  LTDC_InitStruct.LTDC_AccumulatedActiveW = 522;
  /* Configure accumulated active height */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = 283;
  /* Configure total width */
  LTDC_InitStruct.LTDC_TotalWidth = 524; 
  /* Configure total height */
  LTDC_InitStruct.LTDC_TotalHeigh = 285;
 
  /* Configure R,G,B component values for LCD background color */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 
            
  LTDC_Init(&LTDC_InitStruct);
  
/* LTDC initialization end ---------------------------------------------------*/

/* Layer1 Configuration ------------------------------------------------------*/  
    
  /* Windowing configuration */ 
  /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 43 
     Vertical start   = vertical synchronization + vertical back porch     = 12
     Horizontal stop = Horizontal start + window width -1 = 43 + 480 -1 
     Vertical stop   = Vertical start + window height -1  = 12 + 272 -1      */ 
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 43;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (480 + 43 - 1); 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 12;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (272 + 12 - 1);
  
  /* Pixel Format configuration*/           
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB4444;
  
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  
  /* Configure blending factors */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;  
  
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;   
   
  /* Input Address configuration */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)&ARGB4444_480x272;
  
  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3 
     Active high width         = 480 
     number of bytes per pixel = 2    (pixel_format : ARGB4444) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((480 * 2) + 3);
  
  /*  the pitch is the increment from the start of one line of pixels to the 
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     
  */
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (480 * 2);  
  
  /* configure the number of lines */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 272;
   
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

/* Layer1 Configuration end --------------------------------------------------*/

/* Color Keying configuration ------------------------------------------------*/

  LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 255;
  LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = 255;
  LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = 0;
  
/* Color Keying Configuration end --------------------------------------------*/  

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
  * @brief GPIO configuration for LTDC.
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


/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
