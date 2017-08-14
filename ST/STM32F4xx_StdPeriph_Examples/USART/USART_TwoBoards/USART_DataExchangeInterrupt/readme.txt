/**
  @page USART_DataExchangeInterrupt USART Communication Boards Data Exchange using Interrupt example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    USART/USART_TwoBoards/USART_DataExchangeInterrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the USART Communication Boards Interrupt Data Exchange 
  *          using Interrupt example.
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

This example shows how to use joystick buttons to trigger USART communication 
using interrupts and though using USART firmware library.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
BoardA and BoardB) then connect these two boards through UART lines and GND.

@verbatim
*------------------------------------------------------------------------------*
|                 BoardA                                BoardB                 |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |   UART   |____|TX_____________RX|____|   UART   |    |         |
|        |    |  Device1 |____|RX_____________TX|____|  Device2 |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |                    |                 |                    |         |
|        |  O LED1            |                 |  O LED1            |         |
|        |  O LED2    TAMPER  |                 |  O LED2    TAMPER  |         |
|        |  O LED3        _   |                 |  O LED3        _   |         |
|        |  O LED4       |_|  |                 |  O LED4       |_|  |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

 @note The connection between the pins should use a common Ground.

- Software Description

At TAMPER button press, the USART Transmitter Board sends the TxBuffer to 
the USART Receiver Board

Received data correctness is signaled by LED lightening and though as follows:
 - LED2 is ON when the comparison result between source buffer and destination 
   buffer is passed otherwise LED3 is ON.
  
The steps described above can be also initiated and ensured by BoardB. 

In both boards(BoardA or BoardB), the data transfers is managed using USARTx_IRQHandler 
in stm32f4xx_it.c file.

The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle the LED1 each 100ms indicating that the 
firmware is running.
These operations can be repeated infinitely.


@par Directory contents 
  
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/stm32f4xx_conf.h     Library Configuration file
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/stm32f4xx_it.c       Interrupt handlers
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/stm32f4xx_it.h       Interrupt handlers header file
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/main.c               Main program
  - USART/USART_TwoBoards/USART_DataExchangeInterrupt/main.h               Main program header file


@par Hardware and Software environment
 
  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up
    - Use LED1, LED2 and LED3 connected respectively to PG.06, PG.08 and 
      PI.09 pins
    - Use the TAMPER button 
    - Connect BoardA USART3 TX pin (PC.10) to BoardB USART3 RX pin (PC.11)
    - Connect BoardA USART3 RX pin (PC.11) to BoardB USART3 TX pin (PC.10)
    
  - STM32439I-EVAL RevB Set-up
    - Use LED1, LED2 and LED3 connected respectively to PG.06, PG.07 and
      PG.10 pins
    - Use the TAMPER button 
    - Connect BoardA USART3 TX pin (PA.09) to BoardB USART3 RX pin (PA.10)
    - Connect BoardA USART3 RX pin (PA.10) to BoardB USART3 TX pin (PA.09)


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
