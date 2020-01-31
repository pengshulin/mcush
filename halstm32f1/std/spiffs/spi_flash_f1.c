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

static uint8_t _3bytes_status;
static uint8_t _4bytes_address;

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
    SPI_InitTypeDef spi_init;

    sFLASH_LowLevel_Init();
      
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    /*!< SPI configuration */
    spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi_init.SPI_Mode = SPI_Mode_Master;
    spi_init.SPI_DataSize = SPI_DataSize_8b;
    spi_init.SPI_CPOL = SPI_CPOL_High;
    spi_init.SPI_CPHA = SPI_CPHA_2Edge;
    spi_init.SPI_NSS = SPI_NSS_Soft;
    spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

    spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_init.SPI_CRCPolynomial = 7;
    SPI_Init(sFLASH_SPI, &spi_init);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(sFLASH_SPI, ENABLE);

    _3bytes_status = 0;
}


void sFLASH_Unlock(void)
{
    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_SWREN);
    sFLASH_CS_HIGH();

    sFLASH_CS_LOW();
    if( _3bytes_status )
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
    if( _3bytes_status )
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
    if( _3bytes_status )
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

    /* chips memory larger than 32MBytes needs 4 bytes address */
    if( _4bytes_address )
    {
        sFLASH_WriteEnable();
    
        sFLASH_CS_LOW();
        sFLASH_SendByte(sFLASH_CMD_WRITE_EADDR);
        sFLASH_SendByte((SectorAddr & 0xFF000000) >> 24);
        sFLASH_CS_HIGH();
    }

    /*!< Send write enable instruction */
    sFLASH_WriteEnable();
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
    /* chips memory larger than 32MBytes needs 4 bytes address */
    if( _4bytes_address )
    {
        sFLASH_WriteEnable();
    
        sFLASH_CS_LOW();
        sFLASH_SendByte(sFLASH_CMD_WRITE_EADDR);
        sFLASH_SendByte((WriteAddr & 0xFF000000) >> 24);
        sFLASH_CS_HIGH();
    }

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
    sFLASH_SendByte( _4bytes_address ? sFLASH_CMD_READ2 : sFLASH_CMD_READ );

    if( _4bytes_address )
        sFLASH_SendByte((ReadAddr & 0xFF000000) >> 24);

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
        if( Temp0 == 0xEF )  /* not sure if compatiable with other oem */
        {
            _3bytes_status = Temp2 >= 0x18 ? 1 : 0; /* >= 16MBytes */
            _4bytes_address = Temp2 >= 0x19 ? 1 : 0;  /* >= 32MBytes */
        }
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
//void sFLASH_StartReadSequence(uint32_t ReadAddr)
//{
//    /*!< Select the FLASH: Chip Select low */
//    sFLASH_CS_LOW();
//
//    /*!< Send "Read from Memory " instruction */
//    sFLASH_SendByte(sFLASH_CMD_READ);
//
//    /*!< Send the 24-bit address of the address to read from -------------------*/
//    /*!< Send ReadAddr high nibble address byte */
//    sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
//    /*!< Send ReadAddr medium nibble address byte */
//    sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
//    /*!< Send ReadAddr low nibble address byte */
//    sFLASH_SendByte(ReadAddr & 0xFF);
//}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
//uint8_t sFLASH_ReadByte(void)
//{
//    return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
//}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sFLASH_SendByte(uint8_t byte)
{
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

    /*!< Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(sFLASH_SPI, byte);

    /*!< Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(sFLASH_SPI);
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
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

    /*!< Send Half Word through the sFLASH peripheral */
    SPI_I2S_SendData(sFLASH_SPI, HalfWord);

    /*!< Wait to receive a Half Word */
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    /*!< Return the Half Word read from the SPI bus */
    return SPI_I2S_ReceiveData(sFLASH_SPI);
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
    uint8_t flashstatus = 0;

    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Read Status Register" instruction */
    sFLASH_SendByte(sFLASH_CMD_RDSR);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
        /*!< Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
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
    sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK, ENABLE);

    /*!< Enable GPIO clocks */
    /* all pins have been configured before */
    //RCC_APB2PeriphClockCmd(sFLASH_SPI_SCK_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK | 
    //                       sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_CS_GPIO_CLK, ENABLE);
    
    /*!< SPI pins configuration *************************************************/

    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
          
    /*!< SPI SCK pin configuration */
    gpio_init.GPIO_Pin = sFLASH_SPI_SCK_PIN;
    GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &gpio_init);

    /*!< SPI MOSI pin configuration */
    gpio_init.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
    GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &gpio_init);

    /*!< SPI MISO pin configuration */
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    gpio_init.GPIO_Pin = sFLASH_SPI_MISO_PIN;
    GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &gpio_init);

    /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
    gpio_init.GPIO_Pin = sFLASH_CS_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(sFLASH_CS_GPIO_PORT, &gpio_init);
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
    SPI_Cmd(sFLASH_SPI, DISABLE);
    
    /*!< DeInitializes the sFLASH_SPI *******************************************/
    SPI_I2S_DeInit(sFLASH_SPI);
    
    /*!< sFLASH_SPI Periph clock disable ****************************************/
    sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK, DISABLE);
        
    /*!< Configure all pins used by the SPI as input floating *******************/
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

    gpio_init.GPIO_Pin = sFLASH_SPI_SCK_PIN;
    GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &gpio_init);

    gpio_init.GPIO_Pin = sFLASH_SPI_MISO_PIN;
    GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &gpio_init);

    gpio_init.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
    GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &gpio_init);

    gpio_init.GPIO_Pin = sFLASH_CS_PIN;
    GPIO_Init(sFLASH_CS_GPIO_PORT, &gpio_init);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
