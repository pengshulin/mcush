/**
  @page USART_HyperTerminal USART Hyperterminal example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    USART/USART_HyperTerminal/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the USART Hyperterminal example.
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

This example shows how to use the EVAL_COM1 Transmit and Receive interrupts to 
communicate with the hyperterminal.

First, the EVAL_COM1 sends the TxBuffer to the hyperterminal and still waiting for
a string from the hyperterminal that you must enter.
The string that you have entered is stored in the RxBuffer array. The receive
buffer have a RxBufferSize bytes as maximum (the reception is stopped when this
maximum receive value is reached).

The EVAL_COM1 is configured as follows:
    - BaudRate = 9600 baud  
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - Two Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

 @note When the parity is enabled, the computed parity is inserted at the MSB
       position of the transmitted data.
 
  
@par Directory contents 
  
  - USART/USART_HyperTerminal/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - USART/USART_HyperTerminal/stm32f4xx_conf.h     Library Configuration file
  - USART/USART_HyperTerminal/stm32f4xx_it.c       Interrupt handlers
  - USART/USART_HyperTerminal/stm32f4xx_it.h       Interrupt handlers header file
  - USART/USART_HyperTerminal/main.c               Main program
  - USART/USART_HyperTerminal/main.h               Main program header file


@par Hardware and Software environment
  
  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN16 (USART3) and PC serial port if you want to display data on the HyperTerminal.
      @note Make sure that jumper JP22 is in position 1-2
      
  - STM324x9I-EVAL RevB Set-up 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN8 (USART1) and PC serial port if you want to display data on the HyperTerminal.
      @note Make sure that jumper JP7 is in position 1-2
     
  - Hyperterminal configuration:
    - Word Length = 7 Bits
    - Two Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
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
