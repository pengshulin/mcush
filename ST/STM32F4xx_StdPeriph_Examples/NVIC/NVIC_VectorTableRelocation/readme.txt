/**
  @page NVIC_VectorTableRelocation NVIC Vector Table Relocation example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    NVIC/NVIC_VectorTableRelocation/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the NVIC Vector Table Relocation example.
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

This example shows how to relocate the CortexM4 vector table in a specific 
address other than the default Flash memory base address.
This can be used to build program which will be loaded into Flash memory by an
application previously programmed from the Flash memory base address. 
Such application can be In-Application Programming (IAP) through USART, Ethernet
or USB (Device Firmware Upgrade, DFU).
These applications are available for download from the ST microcontrollers 
website: <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>

To build such application, some special configurations have to be performed:
1)Set the application load address, using your toolchain linker file
2)In the application code, relocate the vector table at the application load 
  address using the "NVIC_SetVectorTable" function from the misc.h/.c driver or
  by modifying the value of the constant "VECT_TAB_OFFSET" defined in "system_stm32f4xx.c" 
  file. 

For example, when building an application to be loaded with the IAP through USART
the application load address and vector table address must be set to 0x08004000.

In this example, the associated program implements a "Delay" function based on
SysTick end of count interrupt, and toggles four LEDs with timing defined by the
"Delay" function.


@par Directory contents 
  
  - NVIC/NVIC_VectorTableRelocation/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - NVIC/NVIC_VectorTableRelocation/stm32f4xx_conf.h     Library Configuration file
  - NVIC/NVIC_VectorTableRelocation/stm32f4xx_it.c       Interrupt handlers
  - NVIC/NVIC_VectorTableRelocation/stm32f4xx_it.h       Interrupt handlers header file
  - NVIC/NVIC_VectorTableRelocation/main.c               Main program
  - NVIC/NVIC_VectorTableRelocation/main.h               Main program header file


@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F42xxx/43xxx devices.
  
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427x/STM32F437x Devices) and 
    STM324x9I-EVAL RevB (STM32F429x/STM32F439x Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins.    

  - STM324x9I-EVAL RevB Set-up
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.07, PG.10
      and PG.12 pins 


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain and setup your project configuration as follows
<ul>
    <li> MDK-ARM
         - In the project option menu, select 'Target' window and enter 0x08004000 
           as IROM1 start address
         - In the project option menu, select 'Linker' window and enter 0x08004000 
           as R/O base address

    <li> EWARM
         - Use "stm32f4xx_flash_offset.icf" as linker file
          
</ul> 

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
