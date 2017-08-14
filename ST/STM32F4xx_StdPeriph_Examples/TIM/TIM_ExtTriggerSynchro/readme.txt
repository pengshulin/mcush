/**
  @page TIM_ExtTriggerSynchro TIM External Trigger Synchro example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_ExtTriggerSynchro/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM External Trigger Synchro example.
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

This example shows how to synchronize TIM peripherals in cascade mode with an
external trigger. In this example three timers are used:

1)TIM1 is configured as Master Timer:
  - Toggle Mode is used
  - The TIM1 Enable event is used as Trigger Output 

2)TIM1 is configured as Slave Timer for an external Trigger connected to TIM1 
  TI2 pin (TIM1 CH2 configured as input pin):
  - The TIM1 TI2FP2 is used as Trigger Input
  - Rising edge is used to start and stop the TIM1: Gated Mode.

3)TIM3 is slave for TIM1 and Master for TIM4:
  - Toggle Mode is used
  - The ITR1(TIM1) is used as input trigger 
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TIM1 enable event).
  - The TIM3 enable event is used as Trigger Output. 

4)TIM4 is slave for TIM3,
  - Toggle Mode is used
  - The ITR2(TIM3) is used as input trigger
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TIM3 enable event).

The TIM1CLK is fixed to 168 MHZ, the Prescaler is equal to 5 so the TIMx clock 
counter is equal to 28 MHz.
The TIM3CLK  and TIM4CLK are fixed to 82 MHZ, the Prescaler is equal to 5 
so the TIMx clock counter is equal to 14 MHz. 
The Three Timers are running at: 
TIMx frequency = TIMx clock counter/ 2*(TIMx_Period + 1) = 189.1 KHz.
   
The starts and stops of the TIM1 counters are controlled by the external trigger.
The TIM3 starts and stops are controlled by the TIM1, and the TIM4 starts and 
stops are controlled by the TIM3.  
  

@par Directory contents 
  
  - RTIM/TIM_ExtTriggerSynchro/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - RTIM/TIM_ExtTriggerSynchro/stm32f4xx_conf.h     Library Configuration file
  - RTIM/TIM_ExtTriggerSynchro/stm32f4xx_it.c       Interrupt handlers
  - RTIM/TIM_ExtTriggerSynchro/stm32f4xx_it.h       Interrupt handlers header file
  - RTIM/TIM_ExtTriggerSynchro/main.c               Main program
  - RTIM/TIM_ExtTriggerSynchro/main.h               Main program header file

 
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
    - Connect an external trigger, with a frequency <= 40KHz, to the TIM1 CH2 
      pin (PE.11). In this example the frequency is equal to 5 KHz.

    - Connect the following pins to an oscilloscope to monitor the different waveforms:
      - TIM1 CH1 (PA.08)
      - TIM3 CH1 (PC.06)
      - TIM4 CH1 (PB.06)
          
  
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
