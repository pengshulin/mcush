/**
  @page USART_Smartcard  USART Smart Card example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    USART/USART_SmartCard/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the USART Smart Card example.
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

This example describes a firmware Smartcard Interface based on the STM32F4xx USART
peripheral. The main purpose of this firmware example is to provide resources 
facilitating the development of an application using the USART peripheral in 
smartcard mode.

The firmware interface is composed of library source files developed in order to
support ISO7816-3/4 specification, an application example is also provided.

This example is based on the AN2598 "Smartcard interface with the STM32F101xx 
and STM32F103xx" application note. For more details, please refer to AN2598 
available on www.st.com


@par Directory contents 

    - USART/USART_Smartcard/system_stm32f4xx.c   STM32F4xx system clock configuration file
    - USART/USART_Smartcard/stm32f4xx_conf.h     Library Configuration file
    - USART/USART_Smartcard/stm32f4xx_it.c       Interrupt handlers
    - USART/USART_Smartcard/stm32f4xx_it.h       Interrupt handlers header file
    - USART/USART_Smartcard/main.c               Main program
    - USART/USART_Smartcard/main.h               Main program header file
    - USART/USART_Smartcard/smartcard.c          Smart Card firmware functions
    - USART/USART_Smartcard/smartcard.h          Smart Card header file
    - USART/USART_Smartcard/platform_config.h    Evaluation board specific configuration file
     

@par Hardware and Software environment
 
  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices)
    evaluation boards and can be easily tailored to any other supported device 
    and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up 
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins.
      @note Make sure that JP21 is closed.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
     - smartcard.c
           
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - smartcard.c
                 
 - Rebuild all files and load your image into target memory
 - Run the example
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
