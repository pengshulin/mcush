/**
  @page RTC_TimeStamp RTC TimeStamp example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_TimeStamp/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC TimeStamp example.
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

This example shows how to use the RTC peripheral and the Time Stamp feature.

One from the following clock can be used as RTC clock source (uncomment the
corresponding define in main.c):
- LSE oscillator clock usually delivered by a 32.768 kHz quartz.
- LSI oscillator clock

The program behaves as following:

1)At startup the program checks the backup data register 0 value:
  - BKP_DR0 value not correct: (RTC_BKP_DR0 value is not correct or has not yet
    been programmed when the program is executed for the first time) the RTC is
    configured,the time  is set to 08:10:00 and the date is set to 12-31-12.
       
  - BKP_DR0 value correct: this means that the RTC is configured and the time
    date and timestamp (time and date) are displayed.

2)When an External Reset occurs the BKP domain is not reset and the RTC configuration
  is not lost.

3)When power on reset occurs:
  - If a battery is connected to the VBAT pin : 
    the BKP domain is not reset and the RTC configuration is not lost.
  - If no battery is connected to the VBAT pin : 
    the BKP domain is reset and the RTC configuration is lost.

4)It configures the RTC TimeStamp pin to be falling edge and enables the
  TimeStamp detection.

5)Applying a low level on the TimeStamp pin (PC.13) by pressing TAMPER push-button,
  the calendar is saved in the time-stamp registers thanks to the timestamp event 
  detection.
   
The example uses LCD to display the current time and timestamp registers contents:
  - Pressing TAMPER push-button, saves the current time and date in RTC TSTR
    and TSDR registers and  displays the current RTC Calendar (Time and date) and
    RTC TimeStamp Calendar (Time and date). LED2 turns ON and LED1 off.
  - Pressing WakeUp push-button, clears the TimeStamp Calendar. LED1 turns ON and
    LED2 off.
    
 
@par Directory contents 

  - RTC/RTC_TimeStamp/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - RTC/RTC_TimeStamp/stm32f4xx_conf.h     Library Configuration file
  - RTC/RTC_TimeStamp/stm32f4xx_it.c       Interrupt handlers
  - RTC/RTC_TimeStamp/stm32f4xx_it.h       Interrupt handlers header file
  - RTC/RTC_TimeStamp/main.c               Main program
  - RTC/RTC_TimeStamp/main.h               Main program header file
                
       
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) 
    evaluation boards and can be easily tailored to any other supported device 
    and development board. 

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use Wakeup push-button. 
    - Use the TAMPER push-button connected to PC.13 pin (EXTI Line13).

       
@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_lcd.c
      
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_lcd.c
       
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
