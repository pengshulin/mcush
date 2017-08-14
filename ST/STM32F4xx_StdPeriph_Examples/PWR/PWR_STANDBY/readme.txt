/**
  @page PWR_STANDBY PWR STANDBY example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    PWR/PWR_Standby/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the PWR STANDBY example.
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

This example shows how to enters the system to STANDBY mode and wake-up from this
mode using: external RESET, RTC Alarm A or WKUP pin.

In the associated software, the system clock is set to 168 MHz, an EXTI line
is configured to generate an interrupt on falling edge and the SysTick is programmed
to generate an interrupt each 250 ms. In the SysTick interrupt handler, the LED1 is
toggled, this is used to indicate whether the MCU is in STANDBY or RUN mode.

When a falling edge is detected on the EXTI line an interrupt is generated. In the 
EXTI handler routine the RTC is configured to generate an Alarm event in 5 second
then the system enters STANDBY mode causing the LED1 to stop toggling. 
A rising edge on WKUP pin or an external RESET will wake-up the system from
STANDBY. If within 5 second neither rising edge on WKUP pin nor external RESET
are generated, the RTC Alarm A will wake-up the system. 

After wake-up from STANDBY mode, program execution restarts in the same way as after
a RESET, the RTC configuration (clock source, prescaler,...) is kept and LED1 restarts
toggling. As result there is no need to configure the RTC.

two leds LED1 and LED2 are used to monitor the system state as following:
 - LED2 ON:       RTC configuration failed (system will go to an infinite loop)
 - LED1 toggling: system in RUN mode
 - LED1 ON :      system in STANDBY mode

These Steps are repeated in an infinite loop.

 @note To measure the current consumption in STANDBY mode, please refer to 
       @subpage PWR_CurrentConsumption example.

 @note This example can not be used in DEBUG mode, this is due to the fact 
       that the Cortex-M4 core is no longer clocked during low power mode 
       so debugging features are disabled.
      

@par Directory contents 

  - PWR/PWR_STANDBY/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - PWR/PWR_STANDBY/stm32f4xx_conf.h     Library Configuration file
  - PWR/PWR_STANDBY/stm32f4xx_it.c       Interrupt handlers
  - PWR/PWR_STANDBY/stm32f4xx_it.h       Interrupt handlers header file
  - PWR/PWR_STANDBY/main.c               Main program
  - PWR/PWR_STANDBY/main.h               Main program header file
  
       
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use LED1 and LED2 connected to PG.06 and PG.08 pins
    - Use the Key push-button connected to pin PG15 (EXTI Line15)

  - STM324x9I-EVAL RevB Set-up
    - Use LED1 and LED2 connected to PG.06 and PG.07 pins
    - Use the Key push-button connected to pin PC15 (EXTI Line15)


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
