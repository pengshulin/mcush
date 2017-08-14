/**
  @page CRYP_AESECBmode  Encrypt and Decrypt data using AES-128 Algo in ECB mode with DMA example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CRYP/CRYP_AESECBmode/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the CRYP AES-128 Algo in ECB mode with DMA example.
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

This example shows how to use the CRYPTO peripheral to encrypt and decrypt data 
using AES-128 Algorithm.

For this example, DMA is used to transfer data from memory to the CRYPTO processor
IN FIFO and also to transfer data from CRYPTO processor OUT FIFO to memory.

The data to be encrypted is a 256-bit data (8 words), which corresponds to 
2x AES-128 Blocks.

This example behaves as below: 
- AES Encryption   (PlainData --> EncryptedData)
- AES Key preparation for decryption (Encryption Key --> Decryption Key)
- AES Decryption (EncryptedData --> DecryptedData)

The Plain data, encrypted data and decrypted data can be displayed on a PC 
HyperTerminal using the USART (CN16).

The USARTx is configured as follows:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled


@par Directory contents 
 
  - CRYP/CRYP_AESECBmode/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CRYP/CRYP_AESECBmode/stm32f4xx_conf.h     Library Configuration file
  - CRYP/CRYP_AESECBmode/stm32f4xx_it.c       Interrupt handlers
  - CRYP/CRYP_AESECBmode/stm32f4xx_it.h       Interrupt handlers header file
  - CRYP/CRYP_AESECBmode/main.c               Main program
  - CRYP/CRYP_AESECBmode/main.h               Main program header file
  
      
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
