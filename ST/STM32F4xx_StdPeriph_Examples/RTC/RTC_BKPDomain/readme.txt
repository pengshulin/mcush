/**
  @page RTC_BKPDomain RTC Backup domain example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    RTC/RTC_BKPDomain/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the RTC Backup domain example.
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

This example shows how to use the peripherals available on Backup Domain. 
These peripherals are: the RCC_BDCR register containing the LSE Osc configuration 
and the RTC Clock enable/disable bits. It embeds also the RTC peripheral with its
associated Backup Data registers. And finally the Backup SRAM(4KB) with its low 
power regulator allowing to preserve its contents when the product is powered by
VBAT pin.

As an application example, it demonstrates how to setup the RTC hardware calendar,
RTC Backup Data registers read/write and BKPSRAM read/write operations. 

One from the following clock can be used as RTC clock source (uncomment the
corresponding define in main.c file): 
  - LSE oscillator clock usually delivered by a 32.768 kHz quartz.
  - LSI oscillator clock
  
The Backup (BKP) domain, still powered by VBAT when VDD is switched off, so its
content is not lost if a battery is connected to the VBAT pin. 
A key value is written in backup data register 0 to indicate if the Backup Domain
is already configured.

The program behaves as follows:

1)After startup the program checks the backup data register 0 value:
  - BKP_DR0 value not correct: (RTC_BKP_DR0 value is not correct or has not yet
    been programmed when the program is executed for the first time) the RTC is
    configured and the user is asked to set the time and date (entered on 
    TFT-LCD) and the RTC Backup Data registers and BKPSRAM are written.
    
  - BKP_DR0 value correct: this means that the RTC is configured and the time
    and date are displayed on LCD. The content of RTC Backup Data registers 
    and BKPSRAM is checked and the result is displayed on color LCD.

2)When an External Reset occurs the BKP domain is not reset.

3)When power on reset occurs:
  - If a battery is connected to the VBAT pin: the BKP domain is not reset and
    the RTC configuration is not lost.
      
  - If no battery is connected to the VBAT pin: the BKP domain is reset and the
    RTC configuration is lost.

4)When an RTC Wakeup event occurs the RTC Wakeup interrupt is generated and the
  current Time/Date is refreshed on LCD.
   
The example uses TFT-LCD and push-button to configure the RTC clock and display
the current calendar:
 
- When the Backup domain is not yet configured:
  - The RCC BDCR Register value is displayed on LCD before and after the Backup
    domain configuration.
  - All RTC Backup Data Registers are written and checked.
  - All BKP SRAM content is written and checked.
  - User asked for RTC Calendar Configuration using the TAMPER/WAKEUP/KEY buttons:
    - Use TAMPER button to increment the current value.
    - Use WAKEUP button to validate the current value.
  - Once configured, the RTC Calendar is displayed on the LCD each second using
    the RTC Wakeup Interrupt.

- When the Backup domain is already configured:
  - No need to reconfigure the RTC Clock and Calendar.
  - All RTC Backup Data Registers are checked.
  - All BKP SRAM content is checked. If the Low power regulator is enabled, the
    BKPSRAM content is not lost.
  - The RTC Calendar is displayed on the LCD each second using
    the RTC Wakeup Interrupt.


@par Directory contents 

  - RTC/RTC_BKPDomain/system_stm32f4xx.c    STM32F4xx system clock configuration file
  - RTC/RTC_BKPDomain/stm32f4xx_conf.h      Library Configuration file
  - RTC/RTC_BKPDomain/stm32f4xx_it.c        Interrupt handlers
  - RTC/RTC_BKPDomain/stm32f4xx_it.h        Interrupt handlers header file
  - RTC/RTC_BKPDomain/main.h                Main header file
  - RTC/RTC_BKPDomain/main.c                Main program
  - RTC/RTC_BKPDomain/lcd_log_conf.h        lcd_log configuration file


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) 
    evaluation boards and can be easily tailored to any other supported device 
    and development board. 

  - STM324xG-EVAL/STM32437I-EVAL Set-up
    - Use the TAMPER push-button connected to PC.13 pin (EXTI Line13).
    - Use the WAKEUP push-button connected to PA.00 pin (EXTI Line0).
    - All messages are displayed on the TFT-LCD.
    - Make sure that jumper JP19 is in position 2-3 to connect the 3V battery to VBAT pin
    
    
@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_lcd.c
     - Utilities\STM32_EVAL\Common\lcd_log.c
      
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_lcd.c
     - Utilities\STM32_EVAL\Common\lcd_log.c
     
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
