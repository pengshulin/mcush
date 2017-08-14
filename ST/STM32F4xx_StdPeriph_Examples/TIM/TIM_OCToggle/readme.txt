/**
  @page TIM_OCToggle TIM OC Toggle example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_OCToggle/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM OC Toggle example.
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

This example shows how to configure the TIM3 peripheral to generate four different 
signals with four different frequencies.

The TIM3CLK frequency is set to SystemCoreClock / 2 (Hz), and we want to get TIM3
counter clock at 21 MHz so the Prescaler is computed as following:
- Prescaler = (TIM3CLK / TIM3 counter clock) - 1
SystemCoreClock is set to 168 MHz for STM32F4xx devices

The TIM3 CCR1 register value is equal to 40961: 
CC1 update rate = TIM3 counter clock / CCR1_Val = 512.68 Hz,
so the TIM3 Channel 1 generates a periodic signal with a frequency equal to 256.35 Hz.

The TIM3 CCR2 register is equal to 20480:
CC2 update rate = TIM3 counter clock / CCR2_Val = 1025.39 Hz
so the TIM3 channel 2 generates a periodic signal with a frequency equal to 512.7 Hz.

The TIM3 CCR3 register is equal to 10240:
CC3 update rate = TIM3 counter clock / CCR3_Val = 2050.8 Hz
so the TIM3 channel 3 generates a periodic signal with a frequency equal to 1025.4 Hz.

The TIM3 CCR4 register is equal to 5120:
CC4 update rate = TIM3 counter clock / CCR4_Val =  4101.56 Hz
so the TIM3 channel 4 generates a periodic signal with a frequency equal to 2050.78 Hz.


@par Directory contents 
  
  - TIM/TIM_OCToggle/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_OCToggle/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_OCToggle/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_OCToggle/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_OCToggle/main.c               Main program
  - TIM/TIM_OCToggle/main.h               Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.
        
  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up
    - Connect the TIM3 pins to an oscilloscope to monitor the different waveforms:
       - PC.06 (TIM3_CH1)
       - PC.07 (TIM3_CH2)
       - PC.08 (TIM3_CH3)
       - PC.09 (TIM3_CH4) 


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
