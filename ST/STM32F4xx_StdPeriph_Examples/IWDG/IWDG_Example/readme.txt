/**
  @page IWDG_Example IWDG example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    IWDG/IWDG_Example/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the IWDG example.
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

This example shows how to update at regular period the IWDG reload counter and 
how to simulate a software fault generating an MCU IWDG reset on expiry of a 
programmed time period.

The IWDG timeout is set to 250 ms (the timeout may vary due to LSI frequency 
dispersion).

First, the TIM5 timer is configured to measure the LSI frequency as the 
LSI is internally connected to TIM5 CH4, in order to adjust the IWDG clock.

The LSI measurement using the TIM5 is described below:
- Configure the TIM5 to remap internally the TIM5 CH4 Input Capture to the LSI
  clock output.
- Enable the TIM5 Input Capture interrupt: after one cycle of LSI clock, the
  period value is stored in a variable and compared to the HCLK clock to get
  its real value. 

Then, the IWDG reload counter is configured as below to obtain 250 ms according 
to the measured LSI frequency after setting the prescaler value:

IWDG counter clock Frequency = LSI Frequency / Prescaler value

The IWDG reload counter is refreshed each 240 ms in the main program infinite 
loop to prevent a IWDG reset.

LED2 is also toggled each 240 ms indicating that the program is running.

An EXTI Line is connected to a GPIO pin, and configured to generate an interrupt
on the rising edge of the signal.

The EXTI Line is used to simulate a software failure: once the EXTI Line event 
occurs, by pressing the TAMPER/KEY push-button, the corresponding interrupt is 
served. In the ISR, a write to invalid address generates a Hardfault exception 
containing an infinite loop and preventing to return to main program (the IWDG 
reload counter is not refreshed).
As a result, when the IWDG counter reaches 00h, the IWDG reset occurs.

If the IWDG reset is generated, after the system resumes from reset, LED1 turns on.
If the EXTI Line event does not occur, the IWDG counter is indefinitely refreshed
in the main program infinite loop, and there is no IWDG reset.


@par Directory contents 
  
  - IWDG/IWDG_Example/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - IWDG/IWDG_Example/stm32f4xx_conf.h     Library Configuration file
  - IWDG/IWDG_Example/stm32f4xx_it.c       Interrupt handlers
  - IWDG/IWDG_Example/stm32f4xx_it.h       Interrupt handlers header file
  - IWDG/IWDG_Example/main.c               Main program
  - IWDG/IWDG_Example/main.h               Main program header file
  
     
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up  
    - Use LED1 and LED2 connected respectively to PG.06 and PG.08 pins.
    - Use the KEY push-button connected to PG.15 pin (EXTI Line15).

  - STM324x9I-EVAL RevB Set-up
    - Use LED1 and LED2 connected respectively to PG.06 and PG.07 pins. 
    - Use the TAMPER/KEY push-button connected to PC.13 pin (EXTI Line13).     
    
    
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
