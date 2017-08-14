/**
  @page I2C_IOExpander  I2C IO Expander example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    I2C/I2C_IOExpander/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the I2C IOExpander example.
  ******************************************************************************
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
  @endverbatim

@par Example Description 

This example shows how to configure and use the IO Expander STMPE1600 
STMPE811 in order to control input Joystick IOs, output IOs and the Touch Screen 
feature. This example provides the elementary steps to control the following modules 
and use them in an application:
The IO pins are used for:
  - Getting Joystick pins status (Polling and interrupt).
  - The Touch Screen controller is used to get the Touched area detection information
    in a single point mode (Polling and interrupt).
    
The input information can be used either in Polling mode (uncomment the define
#define IOE_POLLING_MODE) or in Interrupt mode (uncomment the define #define
IOE_INTERRUPT_MODE). These defines are in main.h file.

After configuring the IO Expander modules and Interrupts (if needed), the output
IOs are set to their default state. If the IO Expander is not operational (or the 
I2C pins are not in the correct level), a message is displayed on the LCD screen.

To get the status of an input:
  - Dedicated function is used for Joystick which returns a JOY_State_TypeDef
    value indicating the currently pressed key. This information is used to update 
    the LCD display (the Joystick pressed key name is displayed).
  - Dedicated function is used for Touch Screen which returns a pointer to 
    a structure holding the Touch Screen status (did a Touch Detection happened, 
    X, Y, and Z positions). Only detection into the drawn rectangles areas is 
    considered and used to update the LEDs status and LCD display (corresponding  
    LED turned ON when its rectangle is touched, and the LED name is displayed on
    the LCD screen).

Besides, the example checks the status of the other Push-Buttons implemented on
the board (TAMPER/KEY and WAKEUP push-buttons). They are checked in parallel 
with the IO Expander IOs and the LCD display and LEDs are updated according to 
the push-buttons state. 


@par Directory contents 
 
  - I2C/I2C_IOExpander/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - I2C/I2C_IOExpander/stm32f4xx_conf.h     Library Configuration file
  - I2C/I2C_IOExpander/stm32f4xx_it.c       Interrupt handlers
  - I2C/I2C_IOExpander/stm32f4xx_it.h       Interrupt handlers header file
  - I2C/I2C_IOExpander/main.c               Main program
  - I2C/I2C_IOExpander/main.h               Main program header file

      
@par Hardware and Software environment  

  - This example runs on and STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.
  
  - This example has been tested with STM324x9I-EVAL RevB board which includes
    the MB1046 LCD board. 


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Select "STM32F429_439xx" workspace related to the used device and Add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe8.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe16.c
      
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
