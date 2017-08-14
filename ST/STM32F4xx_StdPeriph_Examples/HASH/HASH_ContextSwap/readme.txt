/**
  @page HASH_ContextSwap   Context Swapping during Digest calculation example

  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    HASH/HASH_ContextSwap/readme.txt
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of Context Swapping during Digest calculation
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

This example shows how to use the HASH peripheral to save and restore hash 
context to serve a high priority hash operation.

For this example, no DMA is used for data transfer.
The SHA-1 and MD5 digests can be displayed on a PC HyperTerminal using the USART.

- In the first step the MD5 and SHA1 digests are computed and displayed.
- In the second step, a TIM6 interruption is configured to occurs with frequency of 
  280 KHz and a MD5 digest calculation is launched.
  During the TIM6 interrupt routine, the current MD5 context is saved, then a SHA-1
  digest calculation is launched (a high priority hash operation) and a counter is
  incremented (ContextSwapCounter).
  When the SHA1 digest calculation is done, the saved MD5 context is restored and
  the MD5 digest calculation is continued.
  When the MD5 digest calculation is done, both MD5 and SHA1 digests are displayed
  to show that there is no difference between the results of the first and the
  second steps.


@par Directory contents 
  
  - HASH/HASH_ContextSwap/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - HASH/HASH_ContextSwap/stm32f4xx_conf.h     Library Configuration file
  - HASH/HASH_ContextSwap/stm32f4xx_it.c       Interrupt handlers
  - HASH/HASH_ContextSwap/stm32f4xx_it.h       Interrupt handlers header file
  - HASH/HASH_ContextSwap/main.c               Main program
  - HASH/HASH_ContextSwap/main.h               Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F415xx/417xx, STM32F437xx and STM32F439xx devices.
      
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F41xx 
    Devices), STM32437I-EVAL (STM32F437xx Devices) and STM32439I-EVAL RevB 
    (STM32F439xx Devices) evaluation boards and can be easily tailored 
    to any other supported device and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN16 (USART3) and PC serial port.
      @note Make sure that jumper JP22 is in position 1-2
      
  - STM32439I-EVAL RevB Set-up 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN8 (USART1) and PC serial port.
      @note Make sure that jumper JP7 is in position 1-2
      
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
