/**
  @page RTC_Tamper RTC Tamper example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_Tamper/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC Tamper example.
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

This example shows how to write/read data to/from RTC Backup data registers and 
demonstrates the Tamper detection feature.

One from the following clock can be used as RTC clock source (uncomment the
corresponding define in main.c):
  - LSE oscillator clock usually delivered by a 32.768 kHz quartz.
  - LSI oscillator clock

The associated firmware performs the following:
  
1)It configures the RTC_AF1 pin Tamper to be falling edge, and enables the Tamper 
  interrupt.
     
2)It writes the data to all RTC Backup data registers, then check whether the 
  data were correctly written. If yes, LED1 turns ON, otherwise LED3 turns ON.
     
3)When applying a low level on the RTC_AF1 pin (PC.13), the RTC backup data registers
  are reset and the Tamper interrupt is generated. The corresponding ISR then checks
  whether the RTC Backup data registers are cleared or not. If yes LED2 turns ON,
  otherwise LED4 turns ON.


@par Directory contents 

  - RTC/RTC_Tamper/system_stm32f4xx.c    STM32F4xx system clock configuration file
  - RTC/RTC_Tamper/stm32f4xx_conf.h      Library Configuration file
  - RTC/RTC_Tamper/stm32f4xx_it.c        Interrupt handlers
  - RTC/RTC_Tamper/stm32f4xx_it.h        Interrupt handlers header file
  - RTC/RTC_Tamper/main.c                Main program
  - RTC/RTC_Tamper/main.h                Main program header file


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
