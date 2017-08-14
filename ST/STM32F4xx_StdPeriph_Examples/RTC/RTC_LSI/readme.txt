/**
  @page RTC_LSI RTC Hardware Calendar example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_LSI/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC Hardware Calendar example.
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

This example shows how to use the LSI clock source auto calibration to get 
a precise RTC clock. 
As an application example, it demonstrates how to configure the TIM5 timer
internally connected to LSI clock output, in order to adjust the RTC prescaler. 

The Low Speed Internal (LSI) clock is used as RTC clock source.
After reset, the RTC prescaler (asynchronous and synchronous) is set with the 
default LSI frequency (32KHz). 
The RTC WakeUp is configured to generate an interrupt each 1s. The WakeUp counter
is clocked by the RTC CK_SPRE signal (1Hz) and its counter is set to 0.
LED1 is toggled inside the RTC WakeUp interrupt each 1s.

The inaccuracy of the LSI clock causes the RTC WakeUp Interrupt to be inaccurate.
The RTC CK_SPRE signal can be monitored by LED1 which is toggled into the RTC 
Wakeup interrupt service routine.

The program waits until TAMPER Push button is pressed to begin the auto calibration 
procedure:
  - Configure the TIM5 to remap internally the TIM5 Channel 4 Input Capture to 
    the LSI clock output.
  - Enable the TIM5 Input Capture interrupt: after one cycle of LSI clock, the
    period value is stored in a variable and compared to the HCLK clock to get
    its real value.
  - The RTC prescaler is adjusted with this LSI frequency value so that the RTC
    CK_SPRE value become more accurate.
  - When calibration is done LED2 is turned ON to indicate the end of this operation.


@par Directory contents 

  - RTC/RTC_LSI/system_stm32f4xx.c    STM32F4xx system clock configuration file
  - RTC/RTC_LSI/stm32f4xx_conf.h      Library Configuration file
  - RTC/RTC_LSI/stm32f4xx_it.c        Interrupt handlers
  - RTC/RTC_LSI/stm32f4xx_it.h        Interrupt handlers header file
  - RTC/RTC_LSI/main.h                Main program header file  
  - RTC/RTC_LSI/main.c                Main program

     
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) 
    evaluation boards and can be easily tailored to any other supported device 
    and development board. 

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use the TAMPER push-button connected to PC.13 pin (EXTI Line13).


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
 
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
