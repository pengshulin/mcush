/**
  @page CRYP_AES_GCM  Encrypt and Decrypt data using AES Algo using GCM chaining mode example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CRYP/CRYP_AES_GCM/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the CRYP AES Algorithm using GCM chaining mode
  *          example
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
using AES with Galois/Counter Mode (GCM).
The AES-GCM mode requires:
1)Plain text which will authenticated and encrypted. It's size must be a multiple
  of 16 bytes. So is the original plain text size is not a multiple of 16 bytes
  it must be padded.
  
2)Header that will be authenticated but not encrypted. The header size must be
  multiple of 16 bytes whatever the key size.
   
3)Initialization vector a value that is used only once within a specified context.
   It must be padded to 16-byte length with LSB set to 2.
    
4)Key is the parameter which determines the cipher text. In this example the key
  size is 128 bits = 16 bytes but it can be tailored to 192 bits and 256 bits.

The AES-GCM provides:
1)Cipher text which is the encryption result of Plaint text. In this example, the
  cipher text is available in OutputText.
  It's size is the same as the plain text.
  
2)TAG: The Message Authentication Code which is used for both message authentication
  and message tampering detection. It's size is always 16 bytes.
      
The Plain data, encrypted data, authentication TAG (MAC) and decrypted data can
be displayed on a PC HyperTerminal using the USART.

The USARTx is configured as follows:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled


@par Directory contents 
  
  - CRYP/CRYP_AES_GCM/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CRYP/CRYP_AES_GCM/stm32f4xx_conf.h     Library Configuration file
  - CRYP/CRYP_AES_GCM/stm32f4xx_it.c       Interrupt handlers
  - CRYP/CRYP_AES_GCM/stm32f4xx_it.h       Interrupt handlers header file
  - CRYP/CRYP_AES_GCM/main.c               Main program
  - CRYP/CRYP_AES_GCM/main.h               Main program header file


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
