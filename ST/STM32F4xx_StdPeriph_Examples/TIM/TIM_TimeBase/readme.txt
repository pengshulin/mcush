/**
  @page TIM_TimeBase TIM Time Base example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM Time Base example.
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

This example shows how to configure the TIM peripheral in Output Compare Timing 
mode with the corresponding Interrupt requests for each channel in order to generate
4 different time bases.

The TIM3CLK frequency is set to SystemCoreClock / 2 (Hz), to get TIM3 counter 
clock at 6 MHz so the Prescaler is computed as following:
- Prescaler = (TIM3CLK / TIM3 counter clock) - 1

SystemCoreClock is set to 168MHz for STM32F4xx Devices.

The TIM3 CC1 register value is equal to 40961, 
CC1 update rate = TIM3 counter clock / CCR1_Val = 146.48 Hz,
so the TIM3 Channel 1 generates an interrupt each 6.8ms

The TIM3 CC2 register is equal to 27309, 
CC2 update rate = TIM3 counter clock / CCR2_Val = 219.7 Hz
so the TIM3 Channel 2 generates an interrupt each 4.55ms

The TIM3 CC3 register is equal to 13654, 
CC3 update rate = TIM3 counter clock / CCR3_Val = 439.4Hz
so the TIM3 Channel 3 generates an interrupt each 2.27ms

The TIM3 CC4 register is equal to 6826, 
CC4 update rate = TIM3 counter clock / CCR4_Val =  878.9 Hz
so the TIM3 Channel 4 generates an interrupt each 1.13ms.

When the counter value reaches the Output compare registers values, the Output 
Compare interrupts are generated and, in the handler routine, 4 pins(PG.06, PG.08,
PI.09 and PC.07) are toggled with the following frequencies: 

- LED1: 73.24Hz (CC1)
- LED2: 109.8Hz (CC2)
- LED3: 219.7Hz (CC3) 
- LED4: 439.4Hz (CC4)


@par Directory contents 
  
  - TIM/TIM_TimeBase/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_TimeBase/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_TimeBase/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_TimeBase/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_TimeBase/main.c               Main program
  - TIM/TIM_TimeBase/main.h               Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up  
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins and connect them on an oscilloscope to show the different 
      Time Base signals.
        

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
