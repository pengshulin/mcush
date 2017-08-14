/**
  ******************************************************************************
  * @file    LTDC/LTDC_PicturesFromSDCard/main.c
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

/** @addtogroup LTDC_PicturesFromSDCard
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS microSD_fatfs;

char* pDirectoryFiles[MAX_BMP_FILES];

uint8_t  ubNumberOfFiles = 0;
uint32_t uwBmplen = 0;
/* Internal Buffer defined in SDRAM memory */
uint32_t uwInternelBuffer = 0xC0260000;

static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/

static void LCD_Config(void);
static void FileSystem_Config(void);
static void SDCard_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
  int main(void)
  {
    uint32_t counter = 0, transparency = 0;
    uint8_t str[30];
     
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f429_439xx.s) before to branch to application main.
     */ 
    
    /* TAMPER button will be used */
    STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE_GPIO); 
    
    /* SysTick end of count event each 10ms */
    SysTick_Config(SystemCoreClock / 1000);

    /* Configure LCD */
    LCD_Config();
    
    /* SD Card Configuration */
    SDCard_Config();    
        
    /* Configure the File System */
    FileSystem_Config();    
      
    /* Get the BMP file names on root directory */
    ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("/PICT", pDirectoryFiles);
    
    if (ubNumberOfFiles == 0)
    {
      for (counter = 0; counter < MAX_BMP_FILES; counter++)
      {
        free(pDirectoryFiles[counter]);
      }
      LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"  No Bitmap files...      ");
      while(1)
      {
      }
    }
      
    while(1)
    {     
      counter = 0;
       
      while ((counter) < ubNumberOfFiles)
      {
        /* Step1 : Display on Foreground layer -------------------------------*/ 
        /* Format the string */
        sprintf ((char*)str, "PICT/%-11.11s", pDirectoryFiles[counter]);
         
        if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0) 
        {  
          /* Format the string */        
          sprintf ((char*)str, "PICT/%-11.11s", pDirectoryFiles[counter]);
            
          /* Set LCD foreground Layer */
          LCD_SetLayer(LCD_FOREGROUND_LAYER);
            
          /* Open a file and copy its content to an internal buffer */
          Storage_OpenReadFile(uwInternelBuffer, (const char*)str);
            
          /* Write bmp file on LCD frame buffer */
          LCD_WriteBMP(uwInternelBuffer);  
          
          /* Configure the transparency for background layer : Increase the transparency */
          for (transparency = 0; transparency < 255; (transparency++))
          {        
            LCD_SetTransparency(transparency);
            
            /* Insert a delay of display */
            Delay(5);
          }
            
          /* Wait for tamper button pressed */
          while (STM_EVAL_PBGetState(BUTTON_TAMPER) == Bit_SET)
          {
          }
            
          /* Configure the transparency for foreground layer : decrease the transparency */
          for (transparency = 255; transparency > 0; transparency--)
          {        
            LCD_SetTransparency(transparency);
            
            /* Insert a delay of display */
            Delay(5);
          }
           
          /* Jump to the next image */  
          counter++;
          
          /* Step2 : Display on Background layer -----------------------------*/
          /* Format the string */  
          sprintf ((char*)str, "PICT/%-11.11s", pDirectoryFiles[counter]);
          
          if ((Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0) || (counter < (ubNumberOfFiles)))
          {         
            /* Connect the Output Buffer to LCD Background Layer  */
            LCD_SetLayer(LCD_BACKGROUND_LAYER);
            
            /* Format the string */  
            sprintf ((char*)str, "PICT/%-11.11s", pDirectoryFiles[counter]);
              
            /* Open a file and copy its content to an internal buffer */
            Storage_OpenReadFile(uwInternelBuffer, (const char*)str);
             
            /* Write bmp file on LCD frame buffer */
            LCD_WriteBMP(uwInternelBuffer);
              
            /* Configure the transparency for background layer : decrease the transparency */  
            for (transparency = 0; transparency < 255; (transparency++))
            {        
              LCD_SetTransparency(transparency);
              
              /* Insert a delay of display */
              Delay(5);
            }
              
            /* wait for tamper button pressed */
            while (STM_EVAL_PBGetState(BUTTON_TAMPER) == Bit_SET)
            {
            }
            
            /* Step3 : -------------------------------------------------------*/              
            /* Configure the transparency for background layer : Increase the transparency */
            for (transparency = 255; transparency > 0; transparency--)
            {        
              LCD_SetTransparency(transparency);
            
              /* Insert a delay of display */
              Delay(5);
            }
            counter++;   
          }
          else if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
          {
            /* Set the Text Color */
            LCD_SetTextColor(LCD_COLOR_RED); 
        
            LCD_DisplayStringLine(LCD_LINE_7, (uint8_t *) str);        
            LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    File type not supported. "); 
            while(1)
            {
            }      
          }        
        }  
      }      
    }      
  }

/**
  * @brief LCD configuration.
  * @retval
  *  None
  */

static void LCD_Config(void)
{
  /* LCD Initialization */ 
  LCD_Init();

  /* LCD Initialization */ 
  LCD_LayerInit();

  /* Enable the LCD */ 
  LCD_DisplayOn(); 
  
  /* Connect the Output Buffer to LCD Background Layer  */
  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  /* Clear the Background Layer */ 
  LCD_Clear(LCD_COLOR_BLACK);
  
  /* Configure the transparency for background : Increase the transparency */
  LCD_SetTransparency(0);
   
  /* Connect the Output Buffer to LCD Foreground Layer  */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  /* Clear the Background Layer */ 
  LCD_Clear(LCD_COLOR_BLACK);
  
  /* Configure the transparency for foreground : Increase the transparency */
  LCD_SetTransparency(100);
}

/**
  * @brief File System Configuration.
  * @retval
  *  None
  */

static void FileSystem_Config(void)
{
  uint32_t counter = 0;
  
  /* Check the mounted device */
  if ( f_mount(0, &microSD_fatfs ) != FR_OK )
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    Cannot mount FATFS on Drive");
  }  
      
  /* Initialize the Directory Files pointers (heap) */
  for (counter = 0; counter < MAX_BMP_FILES; counter++)
  {
    pDirectoryFiles[counter] = malloc(11); 
  }
}

/**
  * @brief  SD Card Configuration.
  * @param  None
  * @retval None
  */
static void SDCard_Config(void)
{
  uint32_t error = 0;
  uint32_t counter = 0x100;
  
  /* Configure the IO Expander */
  if (IOE16_Config() != IOE16_OK)
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
      
    LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)"    IO Expander FAILED         ");
    LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"    Please Reset the board and ");   
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    Start again...             ");
    while(1)
    {
    }
  }   
  /* SDCard initialisation */
  SD_Init();
  
  /* Configure the SD detect pin */
  IOE16_IOPinConfig(SD_DETECT_PIN, Direction_IN);

  if (SD_Detect() == SD_NOT_PRESENT)
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);

    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    Please insert SD Card.     ");

    while (SD_Detect() == SD_NOT_PRESENT)
    {
    }       
  }
  
  /* FAT Initialization */
  do
  {
    /* SDCARD Initialisation */
    error = Storage_Init();                                                    
  }
  while ((error != 0) && (counter-- != 0));

  /* SD Card not formatted */
  if (counter == 0)
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);

    LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"    SD Card not formatted.  ");
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    Reprogram your card.    ");
    while (1)
    {
    }
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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
  {}
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
