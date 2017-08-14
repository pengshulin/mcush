/**
  @page SHA224_SHA256_DMA  HASH SHA-224 and SHA-256 with DMA example

  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    HASH/SHA224_SHA256_DMA/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the HASH SHA-224 and SHA-256 with DMA example.
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

This example shows how to use the HASH peripheral to hash data using SHA-224 
and SHA-256 Algorithms.

For this example, DMA is used to transfer data from memory to the HASH processor.
The message to hash must be multiple of 512-bit (64 bytes) so the message must
be padded to multiple of 64 bytes. In this example, the message to hash is
2048-bit (256 bytes) data.
The SHA-224 message digest result is a 224-bit data (28 bytes = 7 words) available
in H0...H6 registers.
The SHA-256 message digest result is a 256-bit data (32 bytes = 8 words) available
in H0...H7 registers.

The SHA-224 and SHA-256 digests can be displayed on a PC HyperTerminal using the 
USART.


@par Directory contents
  
  - HASH/SHA224_SHA256_DMA/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - HASH/SHA224_SHA256_DMA/stm32f4xx_conf.h     Library Configuration file
  - HASH/SHA224_SHA256_DMA/stm32f4xx_it.c       Interrupt handlers
  - HASH/SHA224_SHA256_DMA/stm32f4xx_it.h       Interrupt handlers header file
  - HASH/SHA224_SHA256_DMA/main.c               Main program
  - HASH/SHA224_SHA256_DMA/main.h               Main program header file  


@par Hardware and Software environment 

  - This example runs on STM32F437xx and STM32F439xx devices.

  - This example has been tested with STMicroelectronics STM32437I-EVAL 
    (STM32F437xx Devices) and STM32439I-EVAL RevB (STM32F439xx Devices) evaluation boards 
    and can be easily tailored to any other supported device and development board.

  - STM32437I-EVAL Set-up 
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
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
   
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
