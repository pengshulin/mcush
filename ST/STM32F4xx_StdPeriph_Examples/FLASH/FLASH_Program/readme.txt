/**
  @page FLASH_Program FLASH Program example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    FLASH/FLASH_Program/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the FLASH Program example.
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

This example provides a description of how to program the FLASH memory integrated 
within STM32F40xx/41xx, STM32F427x/437x and STM32F429x/439x Devices.

After Reset, the Flash memory Program/Erase Controller is locked. To unlock it,
the FLASH_Unlock function is used.
Before programming the desired addresses, an erase operation is performed using 
the flash erase sector feature. The erase procedure starts with the calculation of
the number of sector to be used. Then all these sectors will be erased one by one.

Once this operation is finished, the programming operation will be performed using  
the word programming function. The written data is then checked (read the 
content of memory and compare it to the expected data).
If all the data are correct, the LED1 is ON, otherwise the LED2 is ON.


@par Directory contents 

  - FLASH/FLASH_Program/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - FLASH/FLASH_Program/stm32f4xx_conf.h     Library Configuration file
  - FLASH/FLASH_Program/stm32f4xx_it.c       Interrupt handlers
  - FLASH/FLASH_Program/stm32f4xx_it.h       Interrupt handlers header file
  - FLASH/FLASH_Program/main.c               Main program
  - FLASH/FLASH_Program/main.h               Main program header file

      
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

      
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
