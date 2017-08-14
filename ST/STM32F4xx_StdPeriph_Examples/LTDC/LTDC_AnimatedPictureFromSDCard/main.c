/**
  ******************************************************************************
  * @file    LTDC/LTDC_AnimatedPictureFromSDCard/main.c
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

/** @addtogroup LTDC_AnimatedPictureFromSDCard
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS    microSD_fatfs;
DIR      directory;

char*    pDirectoryFiles[MAX_BMP_FILES];
uint8_t  ubNumberOfFiles = 0;
uint32_t uwBmplen = 0;

/* Internal Buffer defined in SDRAM memory */
uint32_t uwInternelBuffer = 0xC0260000;

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
  uint32_t counter = 0;
  uint8_t  str[30];
 
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f429_439xx.s) before to branch to application main.
     */  
  
  /* Example resources configuration -----------------------------------------*/
  /* Configure LCD */
  LCD_Config();

  /* Configure SD Card */
  SDCard_Config();
  
  /* Configure the File System */
  FileSystem_Config();
 
  /* Display Background picture ----------------------------------------------*/
  
  /* Set Background Layer  */
  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  
  /* Open directory */
  if (f_opendir(&directory, "/BACK") != FR_OK)
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    Open directory.. fails");
    while(1)
    {
    } 
  }

  if (Storage_CheckBitmapFile("BACK/image.bmp", &uwBmplen) == 0)
  {
    /* Format the string */
    Storage_OpenReadFile(uwInternelBuffer, "BACK/image.bmp");
    /* Write bmp file on LCD frame buffer */
    LCD_WriteBMP(uwInternelBuffer);   
  }
  else
  {
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
            
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    File type not supported. "); 
    while(1)
    {
    }  
  }        
        

  /* Display Foreground pictures ---------------------------------------------*/
  /* Set Foreground Layer  */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
 
  /* Decrease the foreground transprency */
  LCD_SetTransparency(200); 
  
  /* Get the BMP file names on root directory */
  ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("/TOP", pDirectoryFiles);

  if (ubNumberOfFiles == 0)
  {
    for (counter = 0; counter < MAX_BMP_FILES; counter++)
    {
      free(pDirectoryFiles[counter]);
    }
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"    No Bitmap files...      ");
    while(1)
    {
    } 
  } 
  
  while(1)
  { 
    counter = 0;
    
    while (counter < ubNumberOfFiles)
    {
      /* Format the string */
      sprintf ((char*)str, "TOP/%-11.11s", pDirectoryFiles[counter]); 
      
      if (Storage_CheckBitmapFile((const char*)str, &uwBmplen) == 0)
      {
        /* Format the string */
        sprintf ((char*)str, "TOP/%-11.11s", pDirectoryFiles[counter]);
        
        /* Open a file and copy its content to a buffer */
        Storage_OpenReadFile(uwInternelBuffer, (const char*)str);
        
        /* Write bmp file on LCD frame buffer */
        LCD_WriteBMP(uwInternelBuffer);
        
        /* jump to next image */
        counter++;   
      }
      else
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

/**
  * @brief LCD configuration.
  * @retval
  *  None
  */

static void LCD_Config(void)
{
  /* LCD Initialization */ 
  LCD_Init();

  /* LCD Layers Initialization */ 
  LCD_LayerInit();
  
  /* Enable the LCD */ 
  LCD_DisplayOn();

  /* Set LCD Background Layer  */
  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  /* Clear the Background Layer */
  LCD_Clear(LCD_COLOR_WHITE);

  /* Set LCD Foreground Layer  */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  /* Clear the Foreground Layer */ 
  LCD_Clear(LCD_COLOR_WHITE); 

  /* Configure and enable the Color Keying feature */
  LCD_SetColorKeying(0); 

  /* Configure the transparency for foreground : Increase the transprency */
  LCD_SetTransparency(100);

}

/**
  * @brief File System Initialisation.
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
