/**
  @page NVIC_IRQ_Priority NVIC IRQ Priority example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    NVIC/NVIC_IRQPriority/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the NVIC IRQ Priority example.
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

This example shows how to use the Nested Vectored Interrupt Controller (NVIC): 

- Configuration of 2 EXTI Lines (WAKEUP button EXTI Line & KEY/TAMPER button EXTI 
  Line) to generate an interrupt on each falling edge and use the SysTick interrupt.
- These interrupts are configured with the following parameters:
  - WAKEUP button EXTI Line:  
    - Preemption Priority = ubPreemptionPriorityValue
    - SubPriority = 0
  - KEY/TAMPER button EXTI Line:    
    - Preemption Priority = 0
    - SubPriority = 1           
  - SysTick Handler:  
    - Preemption Priority = !ubPreemptionPriorityValue
    - SubPriority = 0  
                 
First, the "ubPreemptionPriorityValue" is equal to 0, the WAKEUP button EXTI Line 
has higher preemption priority than the SysTick handler. 

In the KEY/TAMPER button EXTI Line interrupt routine the WAKEUP button EXTI Line and 
SysTick preemption priorities are inverted. 
In the WAKEUP button EXTI Line interrupt routine, the pending bit of the SysTick 
interrupt is set. This will cause SysTick ISR to preempt the WAKEUP button EXTI 
Line ISR only if it has higher preemption priority.

The system behaves as follows:
 
1) The first time KEY/TAMPER button EXTI Line interrupt occurs, the SysTick preemption 
becomes higher than WAKEUP button EXTI Line one. So when the WAKEUP button EXTI 
Line interrupt occurs, the SysTick ISR is executed and the "ubPreemptionOccurred" 
variable become TRUE and the four LEDs (LED1, LED2, LED3, LED4) start toggling.

2) When the next KEY/TAMPER button EXTI Line interrupt occurs, the SysTick preemption
becomes lower than WAKEUP button EXTI Line one. So when the WAKEUP button EXTI 
Line interrupt occurs, the "ubPreemptionOccurred" variable became FALSE and the four 
LEDs (LED1, LED2, LED3, LED4) stop toggling.

Then this behavior is repeated from point 1) in an infinite loop.


@par Directory contents 

  - NVIC/NVIC_IRQPriority/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - NVIC/NVIC_IRQPriority/stm32f4xx_conf.h     Library Configuration file
  - NVIC/NVIC_IRQPriority/stm32f4xx_it.c       Interrupt handlers
  - NVIC/NVIC_IRQPriority/stm32f4xx_it.h       Interrupt handlers header file
  - NVIC/NVIC_IRQPriority/main.c               Main program
  - NVIC/NVIC_IRQPriority/main.h               Main program header file 


@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F42xxx/43xxx devices.
  
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427x/STM32F437x Devices) and 
    STM324x9I-EVAL RevB (STM32F429x/STM32F439x Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins
    - Use the WAKEUP push-button connected to pin PA.0 (EXTI Line0).
    - Use the KEY push-button connected to pin PG15 (EXTI Line15)

  - STM324x9I-EVAL RevB Set-up
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.07, PG.10
      and PG.12 pins 
    - Use the WAKEUP push-button connected to pin PA.0 (EXTI Line0)  
    - Use the TAMPER/KEY push-button connected to PC.13 pin (EXTI Line13).    
    
      
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
