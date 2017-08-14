/**
  ******************************************************************************
  * @file    I2C/I2C_IOExpander/main.c 
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

/** @addtogroup I2C_IOExpander
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
    
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
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main.
     */     

  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
         
  /* Initialize LEDs mounted on EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Select the Button test mode (polling or interrupt) BUTTON_MODE in main.h */
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE);
  STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE);

  /* Initialize the LCD */
  LCD_Init();

  /* Initialize the LCD Layers */
  LCD_LayerInit();  
  
  /* Enable LCD display */
  LCD_DisplayOn();
  
  /* Set foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  
  /* Clear the LCD */ 
  LCD_Clear(White);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(White);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(Blue);    
 
  LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)"        STM324x9I-EVAL       ");
  LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)"      Example on how to      ");
  LCD_DisplayStringLine(LCD_LINE_2, (uint8_t *)"     use the IO Expander     ");
  
  /* Configure the IO Expander */
  if (IOE_Config() == IOE_OK && IOE16_Config() == IOE16_OK)
  {
    LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *)"      IO Expander OK       ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"IO Expander FAILED    ");
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)" Please Reset the     ");
    LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)"   board and start    ");
    LCD_DisplayStringLine(LCD_LINE_7, (uint8_t *)"    again             ");
    while(1);
  }

  /* LEDs Control blocks */
  LCD_SetTextColor(Blue);
  LCD_DrawRect(310, 180,  40, 60);
  LCD_SetTextColor(Red);
  LCD_DrawRect(230, 180, 40, 60);
  LCD_SetTextColor(Yellow);
  LCD_DrawRect(150, 180, 40, 60);
  LCD_SetTextColor(Green);
  LCD_DrawRect(70, 180, 40, 60);

#ifdef IOE_INTERRUPT_MODE
  /* Configure motherboard interrupt source IO_EXP4 */ 
  IOE16_IOPinConfig(IOE16_TS_IT,Direction_IN);
  IOE16_ITConfig(IOE16_TS_IT);
  
  /* Enable joystick interrupt */
  IOE16_ITConfig(JOY_IO16_PINS);
  
  /* Enable the Touch Screen interrupt */
  IOE_TSITConfig(); 
  
  /* Read IOs state to let IO interrupt occur */
  IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPMR_LSB);
  IOE16_I2C_ReadDeviceRegister(IOE16_REG_GPMR_MSB);
#endif /* IOE_INTERRUPT_MODE */
  
  
  while(1)
  {
#ifdef IOE_POLLING_MODE
    static JOY_State_TypeDef JoyState = JOY_NONE;
    static TS_STATE* TS_State;
    
    /* Get the Joystick State */
    JoyState = IOE16_JoyStickGetState();
    
    /* Set the LCD Text Color */
    LCD_SetTextColor(Blue); 
   
    switch (JoyState)
    {
      case JOY_NONE:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:     ----        ");
        break;
      case JOY_UP:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:     UP         ");
        break;     
      case JOY_DOWN:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:    DOWN        ");
        break;          
      case JOY_LEFT:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:    LEFT        ");
        break;         
      case JOY_RIGHT:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:    RIGHT        ");
        break;                 
      case JOY_CENTER:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:   CENTER       ");
        break; 
      default:
        LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"JOY:   ERROR      ");
        break;         
    }

    /* Update the structure with the current position */
    TS_State = IOE_TS_GetState();  
    
    if ((TS_State->TouchDetected) && (TS_State->Y < 92) && (TS_State->Y > 52))
    {
      if ((TS_State->X > 60) && (TS_State->X < 120))
      {
        LCD_SetTextColor(LCD_COLOR_GREEN);   
        LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"     LD1                ");
        STM_EVAL_LEDOn(LED1);
      }
      else if ((TS_State->X > 140) && (TS_State->X < 200))
      {
        LCD_SetTextColor(LCD_COLOR_YELLOW); 
        LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"          LD2           ");
        STM_EVAL_LEDOn(LED2);
      }
      else if ((TS_State->X > 220) && (TS_State->X < 280))
      {
        LCD_SetTextColor(LCD_COLOR_RED); 
        LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"               LD3      ");
        STM_EVAL_LEDOn(LED3);
      }     
      else if ((TS_State->X > 300) && (TS_State->X < 360))
      {
        LCD_SetTextColor(LCD_COLOR_BLUE); 
        LCD_DisplayStringLine(LCD_LINE_10, (uint8_t *)"                    LD4 ");
        STM_EVAL_LEDOn(LED4);
      }
    }
    else
    {
      STM_EVAL_LEDOff(LED1);
      STM_EVAL_LEDOff(LED2);
      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED4);
    }
#endif /* IOE_POLLING_MODE */  
    
#ifdef BUTTON_POLLING_MODE
    /* Insert 10 ms delay */
    Delay(1);
    
    /* Set the LCD Text Color */
    LCD_SetTextColor(Blue); 

    if (STM_EVAL_PBGetState(BUTTON_TAMPER) == 0)
    {
      /* Toggle LD2 */
      STM_EVAL_LEDToggle(LED2);

      LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"Pol: TAMPER/KEY Pressed  ");
    }

    if (STM_EVAL_PBGetState(BUTTON_WAKEUP) != 0)
    {
      /* Toggle LD3 */
      STM_EVAL_LEDToggle(LED3);
      LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"Pol: WAKEUP Pressed      ");
    }
#endif
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
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
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)"assert_param error!!");
  
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
