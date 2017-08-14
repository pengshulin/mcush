/**
  @page CEC_DataExchange example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CEC/CEC_DataExchange/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the CEC Data Exchange Example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

This example shows how to configure and use the CEC peripheral to receive and 
transmit messages.


- Hardware Description

To use this example, two STM32446E-EVAL boards (called DEVICE_1 and 
DEVICE_2) are loaded with the same software then connected through CEC lines

 /|\  In the firmware file main.h, uncomment the dedicated line to use
/_!_\ the CEC peripheral as STM32 DEVICE_1 or STM32 DEVICE_2.

@verbatim
*------------------------------------------------------------------------------*
|           STM32446E_EVAL                         STM32446E_EVAL              |
|         Device Address :0x01                    Device Address :0x03         |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |             3.3V   |                 |                    |         | 
|        |               |    |                 |                    |         | 
|        |             27Kohm |                 |                    |         | 
|        |               |    |                 |                    |         | 
|        |         PB3/PB6 O--|-----------------|--O PB3/PB6         |         |
|        |                    |                 |                    |         |
|        |  O LD1             |                 |  O LD1             |         |
|        |  O LD2    Joystick |                 |  O LD2    Joystick |         |
|        |  O LD3        _    |                 |  O LD3        _    |         |
|        |  O LD4       |_|   |                 |  O LD4       |_|   |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------**
@endverbatim


- Software Description

The test unrolls as follows.

On TX side, when User push-button is pressed, The data is transmitted.

Once the  data is received.A comparison is done and TransferStatus variable gives 
the data transfer status where it is PASSED if transmitted and received data are the same 
otherwise it is FAILED.

Practically, 1 EXTI lines (EXTI15_10)  is configured to 
generate an interrupt on each falling or rising edge. 
A specific message is then transmitted by the CEC IP.
    - EXTI15_10 is mapped to PC.13

In this example, HCLK is configured at 180 MHz.

@par Directory contents 

  - CEC/CEC_DataExchange/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CEC/CEC_DataExchange/stm32f4xx_conf.h     Library configuration file
  - CEC/CEC_DataExchange/stm32f4xx_it.c       Interrupt handlers
  - CEC/CEC_DataExchange/stm32f4xx_it.h       Interrupt handlers header file
  - CEC/CEC_DataExchange/main.c               Main program
  - CEC/CEC_DataExchange/main.h               Header for main.c module  
  
@par Hardware and Software environment

  - This example runs on STM32F446xx devices.
    
  - This example has been tested with STM32446E-EVAL RevB board and can be
    easily tailored to any other supported device and development board.      

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - in main.h, uncomment DEVICE_1 for first board, uncomment DEVICE_2 for second board
 - Rebuild all files and load your image into target memory
 - Be aware that PB6 pin is missing but PB3 is connected directly to it (SB23 is closed).
 - With a wire, connect PB3-PB3 between the 2 boards
 - Add a pull-up resistor of 27kohm between PB3 and V3.3
 - Connect the ground of the 2 boards
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
