/**
  @page RTC_StopWatch RTC StopWatch example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_StopWatch/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC StopWatch example.
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

This example shows how to use the STM32F4xx new RTC's sub-seconds and Tamper 
feature (Filter, sampling) features. It simulates a precise chronometer with 10 
record times possibilities stored in the Backup registers:
 - 10 registers for time (second, minutes and hours)
 - 10 registers for subsecond
  
For this example an interactive human interface is developed using STM324xx EVAL’s 
LCD and Push Buttons to allow user to use StopWatch with real time display.

After startup, a default chronometer counter format "00:00:00:000" is displayed 
on the LCD, it corresponds to [Hours]:[minutes]:[seconds]:[milliseconds].
User can manipulate the chronometer features using the TAMPER and WAKEUP buttons:
 - Press WAKEUP button to start counter.
 - Press TAMPER button to save trials in the backup registers (max 10 actions).


@par Directory contents 

  - RTC/RTC_StopWatch/system_stm32f4xx.c    STM32F4xx system clock configuration file
  - RTC/RTC_StopWatch/stm32f4xx_conf.h      Library Configuration file
  - RTC/RTC_StopWatch/stm32f4xx_it.c        Interrupt handlers
  - RTC/RTC_StopWatch/stm32f4xx_it.h        Interrupt handlers header file
  - RTC/RTC_StopWatch/main.c                Main program
  - RTC/RTC_StopWatch/main.h                Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) 
    evaluation boards and can be easily tailored to any other supported device 
    and development board. 

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use Wakeup pash button. 
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
