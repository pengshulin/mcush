/**
  @page TIM_DMABurst TIM DMA Burst transfer example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_DMABurst/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM1 DMA Burst transfer example.
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

This example shows how to update the TIM1 channel1 period and the duty cycle 
using the TIM1 DMA burst feature.

Every update DMA request, the DMA will do 3 transfers of half words into Timer 
registers beginning from ARR register.
On the DMA update request, 0x0FFF will be transferred into ARR, 0x0000 
will be transferred into RCR, 0x0555 will be transferred into CCR1. 

The TIM1CLK frequency is set to SystemCoreClock (Hz), to get TIM1 counter
clock at 24 MHz the Prescaler is computed as following:
- Prescaler = (TIM1CLK / TIM1 counter clock) - 1

SystemCoreClock is set to 168 MHz.

The TIM1 period is 5.8 ms: TIM1 Frequency = TIM1 counter clock/(ARR + 1)
                                          = 24 MHz / 4096 = 5.8 KHz

The TIM1 CCR1 register value is equal to 0x555, so the TIM1 Channel 1 generates a 
PWM signal with a frequency equal to 5.8 KHz and a duty cycle equal to 33.33%:
TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR + 1)* 100 = 33.33%

The PWM waveform can be displayed using an oscilloscope.

 @note No need of RCR update, but we should do it because of the ARR and CCR1 
       mapping.
      

@par Directory contents 
  
  - TIM/TIM_DMABurst/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_DMABurst/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_DMABurst/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_DMABurst/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_DMABurst/main.c               Main program
  - TIM/TIM_DMABurst/main.h               Main program header file

 
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
        - TIM1 CH1 (PA.08) 


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
