/**
  @page TIM_OCActive TIM OC Inactive example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_OCInactive/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM OC Inactive example.
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

This example shows how to configure the TIM peripheral in Output Compare Inactive 
mode with the corresponding Interrupt requests for each channel.

The TIM2CLK frequency is set to SystemCoreClock / 2 (Hz), and the objective is
to get TIM2 counter clock at 2 KHz so the Prescaler is computed as following:
- Prescaler = (TIM2CLK / TIM2 counter clock) - 1
SystemCoreClock is set to 168 MHz for STM32F4xx devices

The TIM2 CCR1 register value is equal to 1000:
TIM2_CC1 delay = CCR1_Val/TIM2 counter clock  = 500 ms
so the PA.00  is reset after a delay equal to 500 ms.

The TIM2 CCR2 register value is equal to 500:
TIM2_CC2 delay = CCR2_Val/TIM2 counter clock = 250 ms
so the PA.01  is reset after a delay equal to 250 ms.

The TIM2 CCR3 register value is equal to 250:
TIM2_CC3 delay = CCR3_Val/TIM2 counter clock = 125 ms
so the PA.02  is reset after a delay equal to 125 ms.

The TIM2 CCR4 register value is equal to 125:
TIM2_CC4 delay = CCR4_Val/TIM2 counter clock = 62.5 ms
so the PA.03  is reset after a delay equal to 62.5 ms.

While the counter is lower than the Output compare registers values, which 
determines the Output delay, the PG.06, PG.08, PI.09 and PC.07 pin are turned ON. 

When the counter value reaches the Output compare registers values, the Output 
Compare interrupts are generated and, in the handler routine, these pins are turned OFF.


@par Directory contents 
  
  - TIM/TIM_OCInactive/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_OCInactive/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_OCInactive/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_OCInactive/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_OCInactive/main.c               Main program
  - TIM/TIM_OCInactive/main.h               Main program header file

     
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
    - Use LED1, LED2, LED3 and LED4 and connect them an oscilloscope to monitor 
      the different waveforms.
      
  
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
