/**
  @page EXTI_Example EXTI Configuration example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    EXTI/EXTI_Example/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the EXTI example.
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

This example shows how to configure external interrupt lines.
In this example, 2 EXTI lines (EXTI Line0 and Line15) are configured  to generate
an interrupt on each rising and falling edge, respectively. 
In the interrupt routine a LED connected to a specific GPIO pin is toggled.

In this example:
- EXTI Line0 is connected to PA0 pin
- EXTI Line15 is connected to PG15 pin

After EXTI configuration, a software interrupt is generated on the EXTI0 toggles LED1.
After that:
- when falling edge is detected on EXTI Line0, LED1 toggles
- when falling edge is detected on EXTI Line15, LED2 toggles

On STM324xG-EVAL/STM32437I-EVAL:
- EXTI Line0 is connected to Wakeup push-button
- EXTI Line15 is connected to Key push-button

 @note In this example, EXTILine15_Config() function can be substituted by 
       STM_EVAL_PBInit(Button_KEY, Mode_EXTI) provided in the STM324xG-EVAL/
       STM32437I-EVAL/STM324x9I-EVAL driver.


@par Directory contents 
  
  - EXTI/EXTI_Example/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - EXTI/EXTI_Example/stm32f4xx_conf.h     Library Configuration file
  - EXTI/EXTI_Example/stm32f4xx_it.c       Interrupt handlers
  - EXTI/EXTI_Example/stm32f4xx_it.h       Interrupt handlers header file
  - EXTI/EXTI_Example/main.c               Main program
  - EXTI/EXTI_Example/main.h               Main program header file


@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.    

  - STM324xG-EVAL/STM32437I-EVAL Set-up  
    - Use LED1 and LED2 connected respectively to PG.06 and PG.08 pins
    - Use the Key push-button connected to pin PG.15 (EXTI Line15)
    - Use the Wakeup push-button connected to pin PA.0 (EXTI Line0)

  - STM324x9I-EVAL RevB Set-up
    - Use LED1 and LED2 connected respectively to PG.06 and PG.07 pins
    - Use the TAMPER/KEY push-button connected to PC.13 pin (EXTI Line13)     
    - Use the WAKEUP push-button connected to pin PA.0 (EXTI Line0)  

    
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
