/**
  @page UDP-TCP echo server netconn demonstration Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2013 STMicroelectronics *******************
  * @file    UDP-TCP echo server netconn/readme.txt
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Description of the STM32F4x7 UDP-TCP echo server netconn demonstration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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

@par Description

This directory contains a set of sources files that implement a UDP-TCP echo server
demonstration for STM32F4x7 devices. 

Please note that for UDP/TCP echo server netconn demonstration, LwIP v1.4.1 is used as
the TCP/IP stack and FreeRTOS v7.3.0 is used as the Real Time Kernel. 

@par Project Directory contents 

 - "inc": contains the demonstration firmware header files
    - inc/main.h               main config file
    - inc/stm32f4x7_eth_bsp.h  header for stm32f4x7_eth_bsp.c
    - inc/netconf.h            header for netconf.c 
    - inc/lwipopts.h           LwIP stack configuration options
    - inc/FreeRTOSConfig.h     FreeRTOS configuration options
    - inc/stm32f4xx_conf.h     library Configuration file 
    - inc/stm32f4x7_eth_conf.h STM32 Ethernet driver Configuration file
    - inc/stm32f4xx_it.h       header for stm32f4xx_it.c
    - inc/serial_debug.h       header for serial_debug.c

 - "src": contains the demonstration firmware source files
    - src/main.c               main program file
    - src/stm32f4x7_eth_bsp.c  STM32F4x7 Ethernet hardware configuration
    - src/netconf.c            LwIP stack initializations
    - src/system_stm32f4xx.c   STM32 system clock configuration file
    - src/stm32f4xx_it.c       STM32 Interrupt handlers
    - src/udpecho.c            udp echo server main task
    - src/tcpecho.c            tcp echo server main task
    - src/serial_debug.c       retarget the printf function to the USART
  
  - "EWARM": contains preconfigured project for EWARM toolchain
 - "MDK-ARM": contains preconfigured project for MDK-ARM toolchain
 - "RIDE": contains preconfigured project for RIDE toolchain
 - "TASKING": contains preconfigured project for TASKING toolchain
 - "TrueSTUDIO": contains preconfigured project for TrueSTUDIO toolchain

@par Hardware and Software environment
  
  - This example has been tested with the following environments:
     - STM324xG-EVAL board
     - STM324x7I-EVAL board
     - Http clients: Firefox Mozilla (v14.0.1 and later) or Microsoft Internet Explorer (v6 and later)
     - DHCP server: PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server
  
    -  STM32 Eval boards Set-up (STM324xG-EVAL and STM324x7I-EVAL):
    - Connect the eval board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)
  - STM32 EVAL boards jumpers setting
        +==========================================================================================+
        +  Jumper |       MII mode configuration            |     RMII mode configuration(*)       +
        +==========================================================================================+
        +  JP5    | 2-3 provide 25MHz clock by MCO(PA8)     |  Not fitted                          +
        +         | 1-2 provide 25MHz clock by ext. Crystal |                                      +
        + -----------------------------------------------------------------------------------------+
        +  JP6    |          2-3                            |  1-2                                 +
        + -----------------------------------------------------------------------------------------+
        +  JP8    |          Open                           |  Close                               +
        + -----------------------------------------------------------------------------------------+
        +  JP22   | 1-2: RS232 is enabled                                                          +
        +==========================================================================================+
    (*) User have to provide the 50 MHz clock by soldering a 50 MHz oscillator (ref SM7745HEV-50.0M or
        equivalent) on the U3 footprint located under CN3 and also removing jumper on JP5. This oscillator
        is not provided with the board. 
        For more details, please refer to STM3240G-EVAL or STM32437I-EVAL evaluation board User manuals.
    @Note: the default setting is MII mode, to change it to RMII mode refer to main.h file.


@par How to use it ?

In order to make the program work, you must do the following:
  1. Load the demonstration code in the STM32F4x7 Flash memory (see below)
  2. Refer to "AN3966 LwIP TCP/IP stack demonstration for STM32F4x7xx microcontrollers"
     to know how to use the demonstration

  In order to load the demonstration code, you have do the following:
   - EWARM
      - Open the Project.eww workspace
      - Rebuild all files: Project->Rebuild all
      - Load project image: Project->Debug
      - Run program: Debug->Go(F5)

   - RIDE
      - Open the Project.rprj project
      - Rebuild all files: Project->build project
      - Load project image: Debug->start(ctrl+D)
      - Run program: Debug->Run(ctrl+F9)

   - MDK-ARM
      - Open the Project.uvproj project
      - Rebuild all files: Project->Rebuild all target files
      - Load project image: Debug->Start/Stop Debug Session
      - Run program: Debug->Run (F5)

   - TASKING
      - Open TASKING toolchain.
      - Click on File->Import, select General->'Existing Projects into Workspace'
        and then click "Next". 
      - Browse to TASKING workspace directory and select the project:
          - STM324xG_EVAL: to configure the project for STM32F4xx devices.
      - Rebuild all project files: Select the project in the "Project explorer"
        window then click on Project->build project menu.
      - Run program: Select the project in the "Project explorer" window then click
        Run->Debug (F11)

   - TrueSTUDO
      - Open the TrueSTUDIO toolchain.
      - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
      - Click on File->Import, select General->'Existing Projects into Workspace' and then click "Next".
      - Browse to the TrueSTUDIO workspace directory, select the project:
          - STM324xG_EVAL: to configure the project for STM32F4xx devices
      - Rebuild all project files: Select the project in the "Project explorer"
        window then click on Project->build project menu.
      - Run program: Run->Debug (F11)

 * <h2><center>&copy; COPYRIGHT STMicroelectronics</center></h2>
 */
