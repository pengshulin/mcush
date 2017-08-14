/**
  @page PWR_STOP PWR STOP example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    PWR/PWR_STOP/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the PWR STOP example.
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

This example shows how to enters the system to STOP mode and wake-up from this
mode using RTC Wakeup Timer Event connected to EXTI Line 22.

In the associated software
  - the system clock is set to 168 MHz
  - the EXTI_Line15 is configured to generate interrupt on falling edge
  - the EXTI_Line22 connected internally to the RTC Wakeup event is configured
    to generate an interrupt on rising edge each 4s
  - the SysTick is programmed to generate an interrupt each 250 ms 
In the SysTick interrupt handler, LED2 is toggled, this is used to indicate whether
the MCU is in STOP or RUN mode.

The system enters STOP mode and will wait for the RTC Wakeup event to be 
generated each 4s, or Key push-button is pressed. 
 - If the RTC WakeUp event (EXTI_Line22) is the source of wakeup from STOP, LED1 is toggled.
 - If the KEY button (EXTI_Line15) is the source of wakeup from STOP, LED4 is toggled.

This behavior is repeated in an infinite loop.

LEDs are used to monitor the system state as following:
 - LED2 toggling: system in RUN mode
 - LED1 toggled: system woken up from STOP using RTC WakeUp Interrupt
 - LED4 toggled: system woken up from STOP using EXTI_Line15 (KEY push-button)
 
 @note To measure the current consumption in STOP mode, please refer to 
       @subpage PWR_CurrentConsumption example.

 @note This example can not be used in DEBUG mode, this is due to the fact 
       that the Cortex-M4 core is no longer clocked during low power mode 
       so debugging features are disabled
       

@par Directory contents 
  
  - PWR/PWR_STOP/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - PWR/PWR_STOP/stm32f4xx_conf.h     Library Configuration file
  - PWR/PWR_STOP/stm32f4xx_it.c       Interrupt handlers
  - PWR/PWR_STOP/stm32f4xx_it.h       Interrupt handlers header file
  - PWR/PWR_STOP/main.c               Main program
  - PWR/PWR_STOP/main.h               Main program header file
  
       
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily tailored to any 
    other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use LED1, LED2 and LED4 connected respectively to PG.06, PG.08 and PC.07 pins
    - Use the Key push-button connected to pin PG15 (EXTI_Line15)

  - STM324x9I-EVAL RevB Set-up
    - Use LED1, LED2 and LED4 connected respectively to PG.06, PG.07 and PG.12 pins
    - Use the Key push-button connected to pin PC15 (EXTI_Line15)
    

@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
        
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
          
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
                 
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
