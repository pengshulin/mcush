/**
  @page SAI_AUDIO  SAI Audio features example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    SAI/SAI_Audio/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Description of the SAI Audio features example.
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

This example shows how to implement SAI audio features. It allows to play an 
audio file through the SAI peripheral and using the external codec implemented 
on the STM324x9I_EVAL RevB board.

In this example the SAI input clock, provided by a dedicated PLL (PLLI2S),  is
configured to have an audio sampling frequency at 48 KHz.

This example uses an audio codec driver which consists of three independent layers:
 - Codec Hardware layer: which controls and communicates with the audio codec 
   (WM8994) implemented on the evaluation board.
 - MAL (Media Access Layer): which controls and interfaces with storage media 
   storing or providing the audio file/stream.
 - The high layer: which implements overall control and interfacing API allowing
   to perform basic audio operations (Init, Play, Pause, Resume, Stop, Volume 
   control and audio file pointer management)
  
In this example the audio file is stored in the internal flash memory (Stereo, 
16-bit, 48 KHz). The analog output device is automatically detected (Speaker or 
Headphone) when the Headphone is plugged/unplugged in/from the audio jack of the 
evaluation board. The example also manages information display and control interface
through push-buttons: 
 - When the application is playing audio file:
     + Key   : Volume UP
     + Wakeup: Volume DOWN

This example plays the audio file stored in internal flash memory and replay it
when it reach end of file. But it can be tailored to used different media storage
devices; SDCard (through SDIO), external Memory (through FMC) ...) or to play 
in streaming mode (ie. from USB port in device or host mode). In this case, Circular
DMA mode should be preferred (by enabling the define AUDIO_MAL_MODE_CIRCULAR in
stm324x9i_eval_audio_codec.h file).  

The WM8994 codec supports the TDM format, for this the SAI peripheral is configured 
with 4 time slots (2 time slots left/right channels for headphone and 2 time slots left/right 
channels for speaker).
In the WM8994 configuration the SLOT0 (2 time slots left/right) is dedicated for 
the headphone and SLOT1 (2 time slots left/right) is dedicated for speaker and 
the user can choose to configure the 2 audio stream in the same time or one stream
by output (speaker/headphone) by enabling the following defines SAI_CODEC_SLOT0_1, 
SAI_CODEC_SLOT0 or SAI_CODEC_SLOT1.

List of Known Limitations and more detailed user notes are provided in file 
stm324x9i_eval_audio_codec.c   (under Utilities\STM32_EVAL\STM324x9I_EVAL)

The provided sample audio file (stored in internal flash memory) is extracted from:
 - Artist/Composer: Dan O'Connor
 - Creative Commons license: Attribution 3.0 United States
   Please read the license following this link:
   http://creativecommons.org/licenses/by/3.0/us/


@par Directory contents 
 
  - SAI/SAI_Audio/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - SAI/SAI_Audio/stm32f4xx_conf.h     Library Configuration file
  - SAI/SAI_Audio/stm32f4xx_it.c       Interrupt handlers
  - SAI/SAI_Audio/stm32f4xx_it.h       Interrupt handlers header file
  - SAI/SAI_Audio/main.c               Main program
  - SAI/SAI_Audio/main.h               Main program header file
  - SAI/SAI_Audio/audio_sample.h       Audio Sample file (in tab format)


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
 - Select "STM32F429_439xx" workspace related to the used device and Add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_audio_codec.c
            
 - Rebuild all files and load your image into target memory
 - Run the example
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
