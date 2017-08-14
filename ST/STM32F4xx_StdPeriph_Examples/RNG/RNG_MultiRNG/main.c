/**
  ******************************************************************************
  * @file    RNG/RNG_MultiRNG/main.c
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

/** @addtogroup RNG_MultiRNG
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#if defined (USE_STM324xG_EVAL)
  #define MESSAGE1   "  To Generate 8x32bit RNG, Press Key  >>"
  #define MESSAGE2   "*** RNG  Example ***"
  #define MESSAGE3   "  Press KEY button " 
  #define MESSAGE4   "     to START     "
  #define MESSAGE5   "   [ 0x%08X ]   "
  #define LINENUM            0x13
  #define FONTSIZE         Font8x12

#elif defined (USE_STM324x7I_EVAL) 
  #define MESSAGE1   "  To Generate 8x32bit RNG, Press Key  >>"
  #define MESSAGE2   "*** RNG  Example ***"
  #define MESSAGE3   "  Press KEY button " 
  #define MESSAGE4   "     to START     "
  #define MESSAGE5   "   [ 0x%08X ]   "
  #define LINENUM          0x13
  #define FONTSIZE       Font8x12

#else
  #define MESSAGE1   "  To Generate 8x32bit RNG, Press Key  >>"
  #define MESSAGE2   "******** RNG  Example ********"
  #define MESSAGE3   "       Press KEY button       "
  #define MESSAGE4   "           to START           "
  #define MESSAGE5   "        [ 0x%08X ]        "
  #define LINENUM           0x15
  #define FONTSIZE        Font12x12
  #define BUTTON_KEY      BUTTON_TAMPER
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void RNG_Config(void);
static void Display_Init(void);
static void Display(uint32_t rng, uint8_t line);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
 uint32_t random32bit = 0;
 uint32_t counter = 0;

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

  /* Display init (LCD or/and USART)*/
  Display_Init();
  
  /* Key Button configuration */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* RNG configuration */
  RNG_Config();

  while (1)
  {
    /* Wait until Key button is pressed */
    while(STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
    {
    }
    /* Loop while Key button is maintained pressed */
    while(STM_EVAL_PBGetState(BUTTON_KEY) == RESET)
    {
    }

    for(counter = 0; counter < 8; counter++)
    {
      /* Wait until one RNG number is ready */
      while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
      {
      }

      /* Get a 32bit Random number */       
      random32bit = RNG_GetRandomNumber();

      /* Display the Random number value on the LCD or/and USART */
      Display(random32bit, counter+1);
    }
  }
}

/**
  * @brief  RNG configuration
  * @param  None
  * @retval None
  */
static void RNG_Config(void)
{  
 /* Enable RNG clock source */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

  /* RNG Peripheral enable */
  RNG_Cmd(ENABLE);
}

/**
  * @brief  Display Init (LCD or/and USART)
  * @param  None
  * @retval None
  */
static void Display_Init(void)
{
#ifdef PRINT_ON_USART 

 USART_InitTypeDef USART_InitStructure;
  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  printf("\n\r ========================================\n");
  printf("\n\r ==== Multiple RNG Generator Example ====\n");
  printf("\n\r ======================================== \n\n\r");
  printf("\n\r  Press key button to generate 8 x 32bit random number\n");
#endif

#ifdef PRINT_ON_LCD  
/* Initialize the LCD */
  LCD_Init();

/* Display message on LCD ***************************************************/
#if defined (USE_STM324x9I_EVAL) 
  /* Initialize the LCD Layers */
  LCD_LayerInit();
  /* Enable The Display */
  LCD_DisplayOn(); 
  /* Set LCD Background Layer  */
  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  /* Clear the Background Layer */ 
  LCD_Clear(LCD_COLOR_WHITE);
  
  /* Set LCD Foreground Layer  */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);

  /* Configure the transparency for foreground */
  LCD_SetTransparency(100);
#endif /* USE_STM324x9I_EVAL */  
  
  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Text size */
  LCD_SetFont(&FONTSIZE);

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);
  
  LCD_DisplayStringLine(LINE(LINENUM), (uint8_t*)MESSAGE1);
  LCD_DisplayStringLine(LINE(0x16), (uint8_t*)"                                         ");
  
  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), (uint8_t*)MESSAGE2);

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue); 

  LCD_DisplayStringLine(LINE(3), (uint8_t*)MESSAGE3);
  LCD_DisplayStringLine(LINE(5), (uint8_t*)MESSAGE4);
#endif
}

/**
  * @brief  Displays the Random number value on LCD or/and USART
  * @param  rnumber: random number to display
  * @param  line: LCD line number
  * @retval None
  */
static void Display(uint32_t rnumber, uint8_t line)
{
#ifdef PRINT_ON_LCD
  uint8_t text[50];
#endif

#ifdef PRINT_ON_USART  
  printf("\r [ 0x%08x ]\n", rnumber);
  if (line == 8)
  {  
    printf("\n\r  Press key button to generate 8 x 32bit random number\n");
  }
#endif

#ifdef PRINT_ON_LCD
  sprintf((char*)text, MESSAGE5, rnumber);
  LCD_DisplayStringLine(LINE(line),text);
#endif
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
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
