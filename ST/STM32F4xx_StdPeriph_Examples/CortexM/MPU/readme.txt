/**
  @page CortexM_MPU CortexM MPU example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CortexM4/MPU/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the CortexM MPU example.
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

This example presents the MPU features on STM32F4xx devices and it can be easily
ported to any other STM32 device supporting MPU.

The example purpose is to configure a memory region as privileged read only region
and tries to perform read and write operation in different mode.

If the access is permitted LED1 is toggling. If the access is not permitted, 
a memory management fault is generated and LED2 is ON.
To generate an MPU memory fault exception due to an access right error, uncomment
the following line "PrivilegedReadOnlyArray[0] = 'e';" in the "stm32_mpu.c" file.


@par Directory contents 
 
  - CortexM/MPU/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CortexM/MPU/stm32f4xx_conf.h     Library Configuration file
  - CortexM/MPU/stm32f4xx_it.c       Interrupt handlers
  - CortexM/MPU/stm32f4xx_it.h       Interrupt handlers header file
  - CortexM/MPU/main.c               Main program
  - CortexM/MPU/main.h               Main program header file
  - CortexM/MPU/stm32_mpu.c          Cortex-M4 MPU regions Access rights file
  - CortexM/MPU/stm32_mpu.h          Header for stm32_mpu.c module
  - CortexM/MPU/Linker               Directory containing liker file for each toolchain
       

@par Hardware and Software environment
  
  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx 
    devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) evaluation 
    boards and can be easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain and setup your project configuration as follows
 - Add the required example files
   - stm32_mpu.c
<ul>
- For TrueSTUDIO toolchains you have to follow these instructions
  - In the project properties window, select C/C++ Build->settings node then 
    the C Linker->General node and use "stm32F_flash_ROAarray.ld" as Script File.
    This linker is configured for STM32F4xx. It should be updated to use it
    with other STM32 devices.
- For SW4STM32 toolchain you have to follow these instructions : 
  - In the project properties window, select C/C++ Build->settings then the C Linker->General
    and refer to "STM32F417IGHx_flash_ROAarray.ld" as a linker file.
</ul>    
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
     - stm32_mpu.c
        
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - stm32_mpu.c
     
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
