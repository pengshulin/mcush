/**
  @page TIM_PWMInput TIM PWM Input example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_PWMInput/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the TIM PWM Input example.
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

This example shows how to use the TIM peripheral to measure the frequency and 
duty cycle of an external signal.

The TIMxCLK frequency is set to SystemCoreClock/4 (Hz), the Prescaler is 0 so the 
counter clock is SystemCoreClock/2 (Hz).
SystemCoreClock is set to 168 MHz fro STM32F4xx Devices.

TIM4 is configured in PWM Input Mode: the external signal is connected to 
TIM4 Channel2 used as input pin.
To measure the frequency and the duty cycle we use the TIM4 CC2 interrupt request,
so In the TIM4_IRQHandler routine, the frequency and the duty cycle of the external 
signal are computed. 
The "Frequency" variable contains the external signal frequency:
TIM4 counter clock = SystemCoreClock / 2,
Frequency = TIM4 counter clock / TIM4_CCR2 in Hz, 
The "DutyCycle" variable contains the external signal duty cycle:
DutyCycle = (TIM4_CCR1*100)/(TIM4_CCR2) in %.

The minimum frequency value to measure is 915 Hz (TIM4 counter clock / CCR MAX).


@par Directory contents 
  
  - TIM/TIM_PWMInput/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_PWMInput/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_PWMInput/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_PWMInput/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_PWMInput/main.c               Main program
  - TIM/TIM_PWMInput/main.h               Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F42xxx/43xxx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL(STM32F40xx/41xx Devices) 
    and STM32437I-EVAL (STM32F427x/437x Devices) evaluation boards and can be easily tailored 
    to any other supported device and development board

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
    - Connect the external signal to measure to the TIM4 CH2 pin (PB.07).


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
