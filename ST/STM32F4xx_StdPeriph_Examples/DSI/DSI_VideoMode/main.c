/**
  ******************************************************************************
  * @file    DSI_VideoMode/main.c 
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
#include "displayed_picture_rgb565.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DSI_VideoMode_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Comment this line to use LCD Hardware Backlight on MB1166 Rev.A board 
   Note: In addition, a hardware update should be done to have this feature fully working:
         Remove R1 resistor connected on MB1166 REVA board
 */
#define OTM8008A_BACKLIGHT_SOFTWARE

#define  VSYNCH  1
#define  VBP     16
#define  VFP     15
#define  VACT    800
#define  HSYNCH  4
#define  HBP     44
#define  HFP     46
#define  HACT    480

#define IMAGEFORMAT_RGB565   0
#define IMAGEFORMAT_RGB888   1

#if defined(OTM8008A_BACKLIGHT_SOFTWARE)
/* CABC Management : ie : Content Adaptive Back light Control in IC OTM8009a */
#define  OTM8009A_CMD_WRDISBV               0x51  /* Write Display Brightness command          */
#define  OTM8009A_CMD_WRCTRLD               0x53  /* Write CTRL Display command                */
#define  OTM8009A_CMD_WRCABC                0x55  /* Write Content Adaptive Brightness command */
#define  OTM8009A_CMD_WRCABCMB              0x5E  /* Write CABC Minimum Brightness command     */
#endif /* OTM8008A_BACKLIGHT_SOFTWARE */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;

DSI_InitTypeDef DSI_InitStructure;
DSI_PLLInitTypeDef PLLInitStructure;
static DSI_VidCfgTypeDef VidCfgStructure;
DSI_CmdCfgTypeDef  CmdCfgStructure;
DSI_LPCmdTypeDef   LPCmdStructure;


uint32_t PixClockFreq = 0;
uint8_t reduced_table_row[4] = {0x00, 0x64, 0x01, 0x90};

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);
static void LCD_Config(void);
static void LCD_Reset(void);
static void DSI_Config(void);
static void OTM8009Setup(DSI_TypeDef* DSIx, uint8_t format);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief Main program
  * @param None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s/startup_stm32f469_479xx.s)
       before to branch to application main.
     */
  
  /* SysTick end of count event each 1ms */
  SysTick_Config(SystemCoreClock/ 1000);
  
  /* Toggle Hardware Reset of the DSI LCD using
  its XRES signal (active low) */
  LCD_Reset();
  /* DSI configuration */
  DSI_Config();
  
  /* Initialize the DSI */
  DSI_DeInit(DSI);
  PLLInitStructure.PLLNDIV = 100;
  PLLInitStructure.PLLIDF = DSI_PLL_IN_DIV5;
  PLLInitStructure.PLLODF = DSI_PLL_OUT_DIV1;
  
  DSI_InitStructure.NumberOfLanes = DSI_TWO_DATA_LANES;
  DSI_InitStructure.TXEscapeCkdiv = 0x4;
  DSI_Init(DSI,&DSI_InitStructure, &PLLInitStructure);
    
  /* Configure the Generic interface read-back Virtual Channel ID */
  DSI_SetGenericVCID(DSI, 0);
  
  /* Configure the DSI for Video mode */
  VidCfgStructure.VirtualChannelID = 0;
  VidCfgStructure.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
  VidCfgStructure.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
  VidCfgStructure.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
  VidCfgStructure.ColorCoding = DSI_RGB565;
  VidCfgStructure.Mode = DSI_VID_MODE_BURST;
  VidCfgStructure.PacketSize = HACT;
  VidCfgStructure.NullPacketSize = 0xFFF;
  VidCfgStructure.HorizontalSyncActive = (HSYNCH * 2); /* HSYNCH * (LaneByteClk / PixelClk) */
  VidCfgStructure.HorizontalBackPorch = (HBP * 2); /* HBP * (LaneByteClk / PixelClk) */
  VidCfgStructure.HorizontalLine = ((HACT+HSYNCH+HBP+HFP) * 2); /* Hline * (LaneByteClk / PixelClk) */         
  VidCfgStructure.VerticalSyncActive = VSYNCH;         
  VidCfgStructure.VerticalBackPorch = VBP;          
  VidCfgStructure.VerticalFrontPorch = VFP;         
  VidCfgStructure.VerticalActive = VACT;             
  VidCfgStructure.LPCommandEnable = DSI_LP_COMMAND_ENABLE;            
  VidCfgStructure.LPLargestPacketSize = 64;        
  VidCfgStructure.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
  VidCfgStructure.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
  VidCfgStructure.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;     
  VidCfgStructure.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE; 
  VidCfgStructure.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;  
  VidCfgStructure.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE; 
  DSI_ConfigVideoMode(DSI, &VidCfgStructure);
  
  /* Enable the DSI host and wrapper */
  DSI_Start(DSI);
  
  /* Configure and enable the LTDC */
  LCD_Config();
  
  /* Enable Layer 1 */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
  
  /* Reload configuration of Layer 1 */
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable The LCD */
  LTDC_Cmd(ENABLE);
  
  /* Drive the display */
  OTM8009Setup(DSI, IMAGEFORMAT_RGB565);
  
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief DSI Resources configuration (RCC, GPIO, NVIC...)
  * @param  None
  * @retval None
  */
static void DSI_Config(void)
{
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the DSI clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_DSI, ENABLE);
  
  /* Enable the LTDC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);  
/* Reset the DSI peripheral --------------------------------------------------*/
  DSI_DeInit(DSI);
  
  /* RCC Configuration -------------------------------------------------------*/
  /* PLLSAIP and PLLSAIQ are not used and are set to there reset values */
  RCC_PLLSAIConfig(384, 2, 4, 7);
  
  /* LTDC_CLK = LTDC_CLK(first level)/PLLSAIDIVR */ 
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div2);  
  
  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  /* Wait till PLLSAI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {}
  PixClockFreq  = 30000000;
}

/**
  * @brief LTDC configuration.  
  * @param None
  * @retval None
  */
static void LCD_Config(void)
{
  LTDC_InitTypeDef       LTDC_InitStruct;
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

/* Reset the LTDC peripheral -------------------------------------------------*/
  LTDC_DeInit();
  
/* LTDC Initialization -------------------------------------------------------*/
  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* Initialize the vertical synchronization polarity as active low */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* Initialize the data enable polarity as active low */ 
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* Initialize the pixel clock polarity as input pixel clock */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* Timing configuration */
  /* Horizontal synchronization width = HSYNCH - 1 */     
  LTDC_InitStruct.LTDC_HorizontalSync = HSYNCH - 1;
  /* Vertical synchronization height = VSYNCH - 1 */
  LTDC_InitStruct.LTDC_VerticalSync = VSYNCH - 1;
  /* Accumulated horizontal back porch = HSYNCH + HBP - 1 */
  LTDC_InitStruct.LTDC_AccumulatedHBP = HSYNCH + HBP - 1; 
  /* Accumulated vertical back porch = VSYNCH + VBP - 1 */
  LTDC_InitStruct.LTDC_AccumulatedVBP = VSYNCH + VBP - 1;  
  /* Accumulated active width = HSYNCH + HBP + Active Width - 1 */  
  LTDC_InitStruct.LTDC_AccumulatedActiveW = HSYNCH + HBP + HACT - 1;
  /* Accumulated active height = VSYNCH + VBP + Active Heigh - 1 */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = VSYNCH+ VBP + VACT - 1;
  /* Total width = HSYNCH + HBP + Active Width + HFP - 1 */
  LTDC_InitStruct.LTDC_TotalWidth = HSYNCH + HBP + HACT + HFP - 1; 
  /* Total height = VSYNCH + VBP + Active Heigh + VFP - 1 */
  LTDC_InitStruct.LTDC_TotalHeigh = VSYNCH + VBP + VACT + VFP - 1;
 
  /* Configure R,G,B component values for LCD background color */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 
  
  /* Initialize LTDC */          
  LTDC_Init(&LTDC_InitStruct);

/* Layer1 Configuration ------------------------------------------------------*/  
  /* Windowing configuration */ 
  /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 0 
     Vertical start   = vertical synchronization + vertical back porch     = 0
     Horizontal stop = Horizontal start + window width -1 = 480 
     Vertical stop   = Vertical start + window height -1  = 800      */ 
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = HSYNCH + HBP + 0;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSYNCH + HBP - 1 + 480; 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = VSYNCH + VBP + 0;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = VSYNCH + VBP - 1 + 800;
  
  /* Pixel Format configuration*/           
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;   
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0; 
  /* Configure blending factors */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
   
  /* Start Address configuration : frame buffer is located at FLASH memory */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)&Displayed_Picture_Table;
  
  /* the length of one line of pixels in bytes + 3 then :
     Line Lenth = Active high width x number of bytes per pixel + 3 
     Active high width         = 480 
     number of bytes per pixel = 2    (pixel_format : RGB565) */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((480 * 2) + 3);
  
  /*  the pitch is the increment from the start of one line of pixels to the 
      start of the next line in bytes, then :
      Pitch = Active high width x number of bytes per pixel     */
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (480 * 2);  
  
  /* Configure the number of lines */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 800;
  
  /* Initializes the Layer */ 
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);
}

/**
  * @brief LCD device configuration.
  * @retval None
  */
void OTM8009Setup(DSI_TypeDef* DSIx, uint8_t format)
{
  uint8_t regData1[]  = {0x80,0x09,0x01};
  uint8_t regData2[]  = {0x80,0x09};
  uint8_t regData3[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01};
  uint8_t regData4[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01};
  uint8_t regData5[]  = {0x79,0x79};
  uint8_t regData6[]  = {0x00,0x01};
  uint8_t regData7[]  = {0x85,0x01,0x00,0x84,0x01,0x00};
  uint8_t regData8[]  = {0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00};
  uint8_t regData9[]  = {0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00};
  uint8_t regData10[] = {0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00};
  uint8_t regData11[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData12[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData13[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData14[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData15[] = {0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData16[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00};
  uint8_t regData17[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData18[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  uint8_t regData19[] = {0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00};
  uint8_t regData20[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02};
  uint8_t regData21[] = {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData22[] = {0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00};
  uint8_t regData23[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01};
  uint8_t regData24[] = {0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t regData25[] = {0xFF,0xFF,0xFF};
  uint8_t regData26[] = {0x01,0x22};
  

  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xFF, regData1);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x80);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 2, 0xFF, regData2);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x80);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC4, 0x30);
  Delay(10);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x8A);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC4, 0x40);
  Delay(10);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB1);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0xA9);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x91);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x34);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB4);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC0, 0x50);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 16, 0xE1, regData3);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 16, 0xE2, regData4);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xD9, 0x4E);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x81);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC1, 0x66);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA1);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC1, 0x08);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x92);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x01);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x95);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x34);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 2, 0xD8, regData5);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x94);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x33);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA3);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC0, 0x1B);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x82);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x83);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x81);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC4, 0x83);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA1);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC1, 0x0E);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA6);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 2, 0xB3, regData6);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x80);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 6, 0xCE, regData7);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 14, 0xCE, regData8);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 14, 0xCE, regData9);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xC0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCF, regData10);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xD0);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xCF, 0x00);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x80);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCB, regData11);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x90);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCB, regData12);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCB, regData13);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCB, regData14);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xC0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCB, regData15);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xD0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCB, regData16);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xE0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCB, regData17);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xF0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCB, regData18);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x80);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCC, regData19);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x90);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCC, regData20);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xA0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCC, regData21);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xCC, regData22);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xC0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCC, regData23);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xD0);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 15, 0xCC, regData24);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x81);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC5, 0x66);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0xB6);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xF5, 0x06);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xFF, regData25);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  /* Tear ON */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x35, 0x00);
  
  DSI_LongWrite(DSIx, 0, DSI_DCS_LONG_PKT_WRITE, 2, 0x44, regData26);
  /* Sleep out */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x11, 0x00);
  
  Delay(100);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  if(format)
  {
    /* Pixel format RGB888 */
    DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3A, 0x77);
  }
  else
  { 
    /* Pixel format RGB565 */
    DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3A, 0x55);
  }
  
  #if defined(OTM8008A_BACKLIGHT_SOFTWARE)
  /* Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_WRDISBV, 0x7F); 
  
  /* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_WRCTRLD, 0x2C);
  
  /* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_WRCABC, 0x02);
  
  /* defaut is 0 (lowest Brightness), 0xFF is highest Brightness */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_WRCABCMB, 0xFF);
#endif /* OTM8008A_BACKLIGHT_SOFTWARE */
  /* Display ON */
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x29, 0x00);
  
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x00);
  DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2C, 0x00);
  
  Delay(200);
}

/**
  * @brief  LCD Reset
  *         Hw reset the LCD DSI activating its XRES signal (active low for some time)
  *         and desactivating it later.
  *         This signal is only cabled on Eval Rev B and beyond.
  */
void LCD_Reset(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* GPIOK Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE);
  
  /* Configure PG6 and PG8 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOK, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOK, GPIO_Pin_7);
  Delay(20);
  GPIO_SetBits(GPIOK, GPIO_Pin_7);
}
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
