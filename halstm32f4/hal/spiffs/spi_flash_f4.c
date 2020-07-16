/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/spi_flash.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   This file provides a set of functions needed to manage the SPI M25Pxxx
  *          FLASH memory. 
  *            
  *          ===================================================================      
  *          Notes: 
  *           - There is no SPI FLASH memory available in STM324xG-EVAL board,
  *             to use this driver you have to build your own hardware.     
  *          ===================================================================
  *   
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32 hardware resources (SPI and 
  *          GPIO) are defined in spi_flash.h file, and the initialization is 
  *          performed in sFLASH_LowLevel_Init() function.
  *            
  *          You can easily tailor this driver to any development board, by just
  *          adapting the defines for hardware resources and sFLASH_LowLevel_Init()
  *          function.
  *            
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |     sFLASH    |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          | sFLASH_CS_PIN               | ChipSelect(/S)|    1        |
  *          | sFLASH_SPI_MISO_PIN / MISO  |   DataOut(Q)  |    2        |
  *          |                             |   VCC         |    3 (3.3 V)|
  *          |                             |   GND         |    4 (0 V)  |
  *          | sFLASH_SPI_MOSI_PIN / MOSI  |   DataIn(D)   |    5        |
  *          | sFLASH_SPI_SCK_PIN / SCK    |   Clock(C)    |    6        |
  *          |                             |    VCC        |    7 (3.3 V)|
  *          |                             |    VCC        |    8 (3.3 V)|  
  *          +-----------------------------+---------------+-------------+  
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
  */

/* Includes ------------------------------------------------------------------*/
#include "spi_flash.h"
#include "hal.h"

static uint8_t _chip_large_scale;

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void sFLASH_LowLevel_DeInit(void);
void sFLASH_LowLevel_Init(void); 

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_DeInit(void)
{
    sFLASH_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_Init(void)
{
    LL_SPI_InitTypeDef spi_init;

    sFLASH_LowLevel_Init();
      
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    /*!< SPI configuration */
    spi_init.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi_init.Mode = LL_SPI_MODE_MASTER;
    spi_init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    spi_init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    spi_init.ClockPhase = LL_SPI_PHASE_2EDGE;
    spi_init.NSS = LL_SPI_NSS_SOFT;
    spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    spi_init.BitOrder = LL_SPI_MSB_FIRST;
    spi_init.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    spi_init.CRCPoly = 7;
    LL_SPI_Init(sFLASH_SPI, &spi_init);

    /*!< Enable the sFLASH_SPI  */
    LL_SPI_Enable(sFLASH_SPI);

    _chip_large_scale = 0;
}


void sFLASH_Unlock(void)
{
    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_SWREN);
    sFLASH_CS_HIGH();

    sFLASH_CS_LOW();
    if( _chip_large_scale )
    {
        sFLASH_SendByte(sFLASH_CMD_WRSR);
        sFLASH_SendByte(0x00);
        sFLASH_SendByte(sFLASH_CMD_WRSR2);
        sFLASH_SendByte(0x00);
        sFLASH_SendByte(sFLASH_CMD_WRSR3);
        sFLASH_SendByte(0x00);
    }
    else
    {
        sFLASH_SendByte(sFLASH_CMD_WRSR);
        sFLASH_SendByte(0x00);
        sFLASH_SendByte(0x00);
    }
    sFLASH_CS_HIGH();
}


void sFLASH_Lock(void)
{
    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_SWREN);
    sFLASH_CS_HIGH();
  
    sFLASH_CS_LOW();
    if( _chip_large_scale )
    {
        sFLASH_SendByte(sFLASH_CMD_WRSR);
        sFLASH_SendByte(0x3C);
        sFLASH_SendByte(sFLASH_CMD_WRSR2);
        sFLASH_SendByte(0x00);
        sFLASH_SendByte(sFLASH_CMD_WRSR3);
        sFLASH_SendByte(0x00);
    }
    else
    {
        sFLASH_SendByte(sFLASH_CMD_WRSR);
        sFLASH_SendByte(0x1C);
        sFLASH_SendByte(0x00);
    }
    sFLASH_CS_HIGH();
}


uint32_t sFLASH_ReadStatus(void)
{
    uint32_t status = 0;

    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_RDSR);
    status = sFLASH_SendByte(sFLASH_DUMMY_BYTE); 
    sFLASH_SendByte(sFLASH_CMD_RDSR2);
    status += ((uint32_t)sFLASH_SendByte(sFLASH_DUMMY_BYTE))<<8; 
    if( _chip_large_scale )
    {
        sFLASH_SendByte(sFLASH_CMD_RDSR3);
        status += ((uint32_t)sFLASH_SendByte(sFLASH_DUMMY_BYTE))<<16; 
    }
    sFLASH_CS_HIGH();
    return status;
}


/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void sFLASH_EraseSector(uint32_t SectorAddr)
{
    sFLASH_Unlock();

    /*!< Send write enable instruction */
    sFLASH_WriteEnable();

    /*!< Sector Erase */
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    /*!< Send Sector Erase instruction */
    sFLASH_SendByte(sFLASH_CMD_SE);
    /*!< Send SectorAddr high nibble address byte */
    sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    sFLASH_SendByte(SectorAddr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_EraseBulk(void)
{
    sFLASH_Unlock();

    /*!< Send write enable instruction */
    sFLASH_WriteEnable();

    /*!< Bulk Erase */
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    /*!< Send Bulk Erase instruction  */
    sFLASH_SendByte(sFLASH_CMD_BE);
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    /*!< Enable the write access to the FLASH */
    sFLASH_WriteEnable();

    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    /*!< Send "Write to Memory " instruction */
    sFLASH_SendByte(sFLASH_CMD_WRITE);
    /*!< Send WriteAddr high nibble address byte to write to */
    sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    /*!< Send WriteAddr medium nibble address byte to write to */
    sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    /*!< Send WriteAddr low nibble address byte to write to */
    sFLASH_SendByte(WriteAddr & 0xFF);

    /*!< while there is data to be written on the FLASH */
    while (NumByteToWrite--)
    {
      /*!< Send the current byte */
      sFLASH_SendByte(*pBuffer);
      /*!< Point on the next byte to be written */
      pBuffer++;
    }

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
    count = sFLASH_SPI_PAGESIZE - Addr;
    NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
    NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

    if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
      else /*!< NumByteToWrite > sFLASH_PAGESIZE */
      {
        while (NumOfPage--)
        {
          sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
          WriteAddr +=  sFLASH_SPI_PAGESIZE;
          pBuffer += sFLASH_SPI_PAGESIZE;
        }

        sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
    else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
    {
      if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
      {
        if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
        {
          temp = NumOfSingle - count;

          sFLASH_WritePage(pBuffer, WriteAddr, count);
          WriteAddr +=  count;
          pBuffer += count;

          sFLASH_WritePage(pBuffer, WriteAddr, temp);
        }
        else
        {
          sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
        }
      }
      else /*!< NumByteToWrite > sFLASH_PAGESIZE */
      {
        NumByteToWrite -= count;
        NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
        NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

        sFLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        while (NumOfPage--)
        {
          sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
          WriteAddr +=  sFLASH_SPI_PAGESIZE;
          pBuffer += sFLASH_SPI_PAGESIZE;
        }

        if (NumOfSingle != 0)
        {
          sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
        }
      }
    }
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Read from Memory " instruction */
    sFLASH_SendByte(sFLASH_CMD_READ);

    /*!< Send ReadAddr high nibble address byte to read from */
    sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /*!< Send ReadAddr medium nibble address byte to read from */
    sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    /*!< Send ReadAddr low nibble address byte to read from */
    sFLASH_SendByte(ReadAddr & 0xFF);

    while (NumByteToRead--) /*!< while there is data to be read */
    {
        /*!< Read a byte from the FLASH */
        *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
    }

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
}

/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t sFLASH_ReadID(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "RDID " instruction */
    sFLASH_SendByte(sFLASH_CMD_RDID);

    /*!< Read a byte from the FLASH */
    Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

    /*!< Read a byte from the FLASH */
    Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

    /*!< Read a byte from the FLASH */
    Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    if( Temp )
    {
        if( Temp0 == 0xEF )
            _chip_large_scale = Temp2 >= 0x18 ? 1 : 0;
    }
    return Temp;
}

/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void sFLASH_StartReadSequence(uint32_t ReadAddr)
{
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Read from Memory " instruction */
    sFLASH_SendByte(sFLASH_CMD_READ);

    /*!< Send the 24-bit address of the address to read from -------------------*/
    /*!< Send ReadAddr high nibble address byte */
    sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /*!< Send ReadAddr medium nibble address byte */
    sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    /*!< Send ReadAddr low nibble address byte */
    sFLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t sFLASH_ReadByte(void)
{
    return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sFLASH_SendByte(uint8_t byte)
{
    /*!< Loop while DR register in not emplty */
    while( LL_SPI_IsActiveFlag_TXE(sFLASH_SPI) == 0 );

    /*!< Send byte through the SPI1 peripheral */
    LL_SPI_TransmitData8(sFLASH_SPI, byte);

    /*!< Wait to receive a byte */
    while( LL_SPI_IsActiveFlag_RXNE(sFLASH_SPI) == 0 );

    /*!< Return the byte read from the SPI bus */
    return LL_SPI_ReceiveData8(sFLASH_SPI);
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord)
{
    /*!< Loop while DR register in not emplty */
    while( LL_SPI_IsActiveFlag_TXE(sFLASH_SPI) == 0 );

    /*!< Send byte through the SPI1 peripheral */
    LL_SPI_TransmitData16(sFLASH_SPI, HalfWord);

    /*!< Wait to receive a byte */
    while( LL_SPI_IsActiveFlag_RXNE(sFLASH_SPI) == 0 );

    /*!< Return the byte read from the SPI bus */
    return LL_SPI_ReceiveData16(sFLASH_SPI);
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_WriteEnable(void)
{
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Write Enable" instruction */
    sFLASH_SendByte(sFLASH_CMD_WREN);

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void sFLASH_WaitForWriteEnd(void)
{
    uint16_t flashstatus = 0;

    sFLASH_CS_LOW();
    do
    {
        sFLASH_SendByte(sFLASH_CMD_RDSR);
        flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE); 
    }
    while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_LowLevel_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    /*!< Enable the SPI clock */
    sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK);

    /*!< Enable GPIO clocks */
    //RCC_AHB1PeriphClockCmd(sFLASH_SPI_SCK_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK | 
    //                       sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_CS_GPIO_CLK, ENABLE);
    
    /*!< SPI pins configuration *************************************************/

    /*!< Connect SPI pins to AF5 */  
    //GPIO_PinAFConfig(sFLASH_SPI_SCK_GPIO_PORT, sFLASH_SPI_SCK_SOURCE, sFLASH_SPI_SCK_AF);
    //GPIO_PinAFConfig(sFLASH_SPI_MISO_GPIO_PORT, sFLASH_SPI_MISO_SOURCE, sFLASH_SPI_MISO_AF);
    //GPIO_PinAFConfig(sFLASH_SPI_MOSI_GPIO_PORT, sFLASH_SPI_MOSI_SOURCE, sFLASH_SPI_MOSI_AF);

    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Alternate = sFLASH_SPI_SCK_AF;

    /*!< SPI SCK pin configuration */
    gpio_init.Pin = sFLASH_SPI_SCK_PIN;
    HAL_GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &gpio_init);

    /*!< SPI MOSI pin configuration */
    gpio_init.Alternate = sFLASH_SPI_MOSI_AF;
    gpio_init.Pin =  sFLASH_SPI_MOSI_PIN;
    HAL_GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &gpio_init);

    /*!< SPI MISO pin configuration */
    gpio_init.Alternate = sFLASH_SPI_MISO_AF;
    gpio_init.Pin =  sFLASH_SPI_MISO_PIN;
    gpio_init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &gpio_init);

    /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
    gpio_init.Pin = sFLASH_CS_PIN;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Speed = GPIO_SPEED_HIGH;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(sFLASH_CS_GPIO_PORT, &gpio_init);
}

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_LowLevel_DeInit(void)
{
    GPIO_InitTypeDef gpio_init;

    /*!< Disable the sFLASH_SPI  ************************************************/
    LL_SPI_Disable(sFLASH_SPI);
    
    /*!< DeInitializes the sFLASH_SPI *******************************************/
    LL_SPI_DeInit(sFLASH_SPI);
    
    /*!< sFLASH_SPI Periph clock disable ****************************************/
    sFLASH_SPI_CLK_DEINIT(sFLASH_SPI_CLK);
        
    /*!< Configure all pins used by the SPI as input floating *******************/
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;

    gpio_init.Pin = sFLASH_SPI_SCK_PIN;
    HAL_GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &gpio_init);

    gpio_init.Pin = sFLASH_SPI_MISO_PIN;
    HAL_GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &gpio_init);

    gpio_init.Pin = sFLASH_SPI_MOSI_PIN;
    HAL_GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &gpio_init);

    gpio_init.Pin = sFLASH_CS_PIN;
    HAL_GPIO_Init(sFLASH_CS_GPIO_PORT, &gpio_init);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
