/**
  @page NVIC_WFIMode NVIC DMA in WFI mode example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    NVIC/NVIC_WFIMode/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the NVIC DMA in WFI mode example.
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

This example shows how to enter the system to WFI mode with DMA transfer enabled
and wake-up from this mode by the DMA End of Transfer interrupt.

In the associated software, the system clock is set to 168 MHz, the DMA1 Stream1
channel4 is configured to transfer 10 data from the EVAL_COM1 (USART3 for 
STM32F40xx/STM32F41xx and STM32F427x/STM32F437x devices, and USART1 for 
STM32F429x/STM32F439x devices) data register to a predefined buffer "DST_Buffer", 
and to generate an interrupt at the end of the transfer. The EVAL_COM1 (USART3/1) 
receives data from Hyperterminal.
LED1 is toggled with a frequency depending on the system clock, this is used 
to indicate whether the MCU is in WFI or RUN mode. 

Pressing TAMPER/KEY push-button (generates rising edge on EXTI Line15/13) will 
put the core in the WFI mode, causing the LED1 to stop toggling.
To wake-up from WFI mode you have to send the sequence (0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
from the Hyperterminal to the EVAL_COM1 (USART3/1). These bytes will be transferred 
by the DMA from the EVAL_COM1 (USART3/1) receive data register to the predefined 
buffer, then generates an interrupt which exits the system from WFI mode. 
The LED1 restarts toggling and a LED2 will toggle if the buffer is correctly 
received else a LED3 is toggled.


@par Directory contents 
 
  - NVIC/NVIC_WFIMode/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - NVIC/NVIC_WFIMode/stm32f4xx_conf.h     Library Configuration file
  - NVIC/NVIC_WFIMode/stm32f4xx_it.c       Interrupt handlers
  - NVIC/NVIC_WFIMode/stm32f4xx_it.h       Interrupt handlers header file
  - NVIC/NVIC_WFIMode/main.c               Main program
  - NVIC/NVIC_WFIMode/main.h               Main program header file
  
          
@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use the Key push-button connected to pin PG.15 (EXTI Line15)
    - Use LED1, LED2 and LED3 connected respectively to PG.06, PG.08 and PC.07 pins    
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN16 (USART3) and PC serial port if you want to display data on the HyperTerminal.
      @note Make sure that jumper JP22 is in position 1-2.

  - STM324x9I-EVAL RevB Set-up
    - Use the TAMPER/KEY push-button connected to pin PC.13 (EXTI Line13)
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.07, PG.10
      and PG.12 pins 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN8 (USART1) and PC serial port if you want to display data on the HyperTerminal.
      @note Make sure that jumper JP7 is in position 1-2.  
            
  - Hyperterminal configuration:
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - flow control: None   
 
            
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
