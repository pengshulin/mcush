/**
  @page TIM_OCActive TIM OC Active example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_OCActive/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM OC Active example.
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

This example shows how to configure the TIM peripheral to generate four different 
signals with four different delays.

The TIM3CLK frequency is set to SystemCoreClock / 2 (Hz), and the objective is
to get TIM3 counter clock at 2 KHz so the Prescaler is computed as following:
- Prescaler = (TIM3CLK / TIM3 counter clock) - 1

SystemCoreClock is set to 168 MHz for STM32F4xx Devices.

The TIM3 CCR1 register value is equal to 1000:
TIM3_CH1 delay = CCR1_Val/TIM3 counter clock  = 500 ms
so the TIM3 Channel 1 generates a signal with a delay equal to 500 ms.

The TIM3 CCR2 register value is equal to 500:
TIM3_CH2 delay = CCR2_Val/TIM3 counter clock = 250 ms
so the TIM3 Channel 2 generates a signal with a delay equal to 250 ms.

The TIM3 CCR3 register value is equal to 250:
TIM3_CH3 delay = CCR3_Val/TIM3 counter clock = 125 ms
so the TIM3 Channel 3 generates a signal with a delay equal to 125 ms.

The TIM3 CCR4 register value is equal to 125:
TIM3_CH4 delay = CCR4_Val/TIM3 counter clock = 62.5 ms
so the TIM3 Channel 4 generates a signal with a delay equal to 62.5 ms.

The delay corresponds to the time difference between PG.06 and  TIM3_CHx signal
rising edges.


@par Directory contents 
  
  - TIM/TIM_OCActive/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_OCActive/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_OCActive/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_OCActive/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_OCActive/main.c               Main program
  - TIM/TIM_OCActive/main.h               Main program header file
  

@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up  
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
       - Use LED1 connected to PG.06
       - TIM3_CH1 (PC.06)
       - TIM3_CH2 (PC.07)
       - TIM3_CH3 (PC.08)
       - TIM3_CH4 (PC.09)


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
