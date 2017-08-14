/**
  @page FMC_SDRAM_DMA  FMC SDRAM read/write DMA transfer operations example 
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FMC/FMC_SDRAM_DMA/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the FMC SDRAM read/write DMA transfer operations 
  *          example.
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

This example shows how to configure the FMC and DMA to perform DMA read/write 
transfer operations on the MT48LC2M3B2B5-7E SDRAM memory mounted on 
STM324x9I-EVAL RevB board.
            
This example gives an application of how to configure the DMA as master to perform 
memory to memory transfer to read/write data from/to the SDRAM memory. 

In a first step, the DMA is configured to write data to SDRAM memory with fixed 
configuration. The user can modify the type of transfer (Mode, burst length, size..)
by modifying DMA fields configuration. The end of transfer is handled by DMA stream 
IRQ handler. 
In a second step, the DMA is reconfigured to read the written data from SDRAM 
in order to check its correctness.
 
The user can choose his own configuration by commenting/uncommenting the defines 
for undesired/desired configurations in the memory associate driver's header file,
for example, to change memory bus width configuration to 16 bit, uncomment the 
define for the configuration "FMC_SDMemory_Width_16b" in "stm324x9i_eval_fmc_sdram.h" 
file.

DMA peripheral configuration:
  - DMA stream: DMA2_Stream0
  - DMA channel : DMA_Channel_0
  - Direction: Memory to memory
  - DMA mode: Normal mode
  - Peripheral/Memory Data size: Word
  - Peripheral/Memory burst: INC4
 
The LEDs are configured to indicate the example status:
  - LED1 ON: correct data transfer (PASS). 
  - LED2 ON: incorrect data transfer (FAIL).
  - LED3 ON: DMA transfer complete.  


@par Directory contents
                       
 - FMC/FMC_SDRAM_DMA/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FMC/FMC_SDRAM_DMA/stm32f4xx_conf.h     Library Configuration file
 - FMC/FMC_SDRAM_DMA/stm32f4xx_it.c       Interrupt handlers
 - FMC/FMC_SDRAM_DMA/stm32f4xx_it.h       Interrupt handlers header file
 - FMC/FMC_SDRAM_DMA/main.c               Main program
 - FMC/FMC_SDRAM_DMA/main.h               Main program header file
  
  
@par Hardware and Software environment 

  - This example runs on STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the STM32F429_439xx device and add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
            
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
