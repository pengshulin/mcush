/**
  @page RTC_Timer RTC Timer example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_Timer/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC Timer example.
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

This example shows how to use the RTC peripheral with Alarm sub seconds 
feature to simulate a timer with refresh time equal to 250 ms  (1 second/ 8) * 2).
The RTC is configured to generate sub seconds interrupt each 125ms (will have
8 interrupt per 1 second).
  
For this example an interactive human interface is developed using STM324xx EVAL’s
LCD and Push Buttons to allow user to use Timer with a real progress bar display.

After startup, the timer is equal to 1 Minute (60 Seconds) and by pressing on
the KEY button the progress bar start to be filled for each 2 successive
interrupts (after each 250ms).
After 480 interrupts (60 * 8) the progress bar will be full.

User can manipulate the chronometer features using the TAMPER and WAKEUP buttons:
  - Press KEY button to Start the timer.
  - Press WAKEUP button to Reset the timer.
  - Press TAMPER button to Stop the timer.

 
@par Directory contents 

  - RTC/RTC_Timer/system_stm32f4xx.c    STM32F4xx system clock configuration file
  - RTC/RTC_Timer/stm32f4xx_conf.h      Library Configuration file
  - RTC/RTC_Timer/stm32f4xx_it.c        Interrupt handlers
  - RTC/RTC_Timer/stm32f4xx_it.h        Interrupt handlers header file
  - RTC/RTC_Timer/main.c                Main program
  - RTC/RTC_Timer/main.h                Main program header file

     
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
