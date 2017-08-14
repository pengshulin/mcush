/**
  ******************************************************************************
  * @file    QSPI/QSPI_ExecuteInPlace/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Program Body 
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
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup QSPI_ExecuteInPlace
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#if defined(__CC_ARM)
extern uint32_t Load$$QSPI$$Base;
extern uint32_t Load$$QSPI$$Length;
#elif defined(__ICCARM__)
#pragma section =".qspi"
#pragma section =".qspi_init"
#elif defined(__GNUC__)
extern uint32_t _qspi_init_base;
extern uint32_t _qspi_init_length;
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QSPI_InitTypeDef  QSPI_InitStructure;
QSPI_ComConfig_InitTypeDef  QSPI_ComConfig_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

uint8_t *FlashAddr;
uint32_t MAXTransferSize = 0x00;
__IO TestStatus TransferStatus = FAILED;
/* Private function prototypes -----------------------------------------------*/
static void QSPI_GPIO_Config(void);
static void QSPI_WriteEnable(void);
static void QSPI_AutoPollingMemReady(void);
static void GpioToggle(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file startup_stm32f446xx.s) before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
  uint32_t address = 0, step = 0x00;

  /* Configure QSPI GPIO */
  QSPI_GPIO_Config();
  
  /* Initialize DMA ----------------------------------------------------------*/
  DMA_StructInit(&DMA_InitStructure);
  DMA_DeInit(QSPI_DMA_STREAM);
  
  /*DMA configuration*/
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
/* Workaround for DMA2 Limitation */
#if defined(STM32F412xG) || defined(STM32F413_423xx)
  DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t) & QUADSPI->DR ;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
#else
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & QUADSPI->DR ;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
#endif /* STM32F412xG */
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Channel            = DMA_Channel_3;
  
  /* Initialize QuadSPI ------------------------------------------------------*/
  QSPI_StructInit(&QSPI_InitStructure);
  QSPI_InitStructure.QSPI_SShift    = QSPI_SShift_HalfCycleShift;
#if defined(STM32F412xG) || defined(STM32F413_423xx)
  QSPI_InitStructure.QSPI_Prescaler = 0x00;
  QSPI_InitStructure.QSPI_FSize     = 0x17;
#else
  QSPI_InitStructure.QSPI_Prescaler = 0x01; /* 90 MHZ */
  QSPI_InitStructure.QSPI_FSize     = 0x18;
#endif /* STM32F412xG */
  QSPI_InitStructure.QSPI_CKMode    = QSPI_CKMode_Mode0;
  QSPI_InitStructure.QSPI_CSHTime   = QSPI_CSHTime_2Cycle;
  QSPI_InitStructure.QSPI_FSelect   = QSPI_FSelect_1;
  QSPI_InitStructure.QSPI_DFlash    = QSPI_DFlash_Disable;
  QSPI_Init(&QSPI_InitStructure);
  
  /* Initialize Command Config -----------------------------------------------*/
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure);
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DDRMode     = QSPI_ComConfig_DDRMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DHHC        = QSPI_ComConfig_DHHC_Enable;
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure);
  QSPI_Cmd(ENABLE);
  
  while(1)
  {
    switch(step)
    {
    case 0:
      
      /* Enable write operations ---------------------------------------------*/
      QSPI_Cmd(ENABLE);
      QSPI_WriteEnable();
      
      /* Erasing Sequence ----------------------------------------------------*/
      QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure); 
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode  = QSPI_ComConfig_IMode_1Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_NoData;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Indirect_Write;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = SECTOR_ERASE_CMD;
      QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);
      
      /* Set sector address to erase */
      QSPI_SetAddress(address);  
      
      while(QSPI_GetFlagStatus(QSPI_FLAG_TC) == RESET)
      {}
      step++;
      break;
      
    case 1:      
      /* Configure automatic polling mode to wait for end of erase -----------*/  
        QSPI_AutoPollingMemReady();
#if defined(__CC_ARM)
    FlashAddr = (uint8_t *)(&Load$$QSPI$$Base);
    MAXTransferSize = (uint32_t)(&Load$$QSPI$$Length);
#elif defined(__ICCARM__)
    FlashAddr = (uint8_t *)(__section_begin(".qspi_init"));
    MAXTransferSize = __section_size(".qspi_init");
#elif defined(__GNUC__)
    FlashAddr = (uint8_t *)(&_qspi_init_base);
	MAXTransferSize = (uint32_t)((uint8_t *)(&_qspi_init_length));
#endif		
      step++;
      break;
      
    case 2:
      /* Enable write operations ---------------------------------------------*/
      QSPI_WriteEnable();
      QSPI_DMACmd(ENABLE);
      
      /* Writing Sequence ----------------------------------------------------*/
      QSPI_SetDataLength(MAXTransferSize);
      
      QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure); 
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
#if defined(STM32F412xG) || defined(STM32F413_423xx)
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_4Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = EXT_QUAD_IN_FAST_PROG_CMD;
#else
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_32bit;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = QUAD_IN_FAST_PROG_CMD;
#endif /* STM32F412xG */
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_4Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
      QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
      
      /* DMA channel Tx Configuration */
      DMA_InitStructure.DMA_BufferSize      = MAXTransferSize;
      /* Workaround for DMA2 Limitation on STM32F412xG devices */
#if defined(STM32F412xG) || defined(STM32F413_423xx)
      DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)FlashAddr;
      DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
#else
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)FlashAddr;
      DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
#endif /* STM32F412xG */
      DMA_InitStructure.DMA_Priority        = DMA_Priority_Low;          
      DMA_Init(QSPI_DMA_STREAM, &DMA_InitStructure);
      DMA_Cmd(QSPI_DMA_STREAM, ENABLE);
      
      /* Wait for the end of Transfer */
      while(DMA_GetFlagStatus(QSPI_DMA_STREAM, QSPI_DMA_FLAG_TC) == RESET)
      {}  
      DMA_ClearFlag(QSPI_DMA_STREAM, QSPI_DMA_FLAG_TC);
      QSPI_AbortRequest();
      DMA_Cmd(QSPI_DMA_STREAM, DISABLE);
      QSPI_DMACmd(DISABLE);
      
      step++;
      break;
      
    case 3:
      /* Configure automatic polling mode to wait for end of program ---------*/  
      QSPI_AutoPollingMemReady(); 
      step++;
      break;
      
    case 4:
      /* Reading Sequence ----------------------------------------------------*/
      QSPI_TimeoutCounterCmd(DISABLE);
      QSPI_MemoryMappedMode_SetTimeout(0);
#if defined(STM32F412xG) || defined(STM32F413_423xx)
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize       = QSPI_ComConfig_ADSize_24bit;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins          = QUAD_INOUT_FAST_READ_CMD;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles  = DUMMY_CLOCK_CYCLES_READ;
#else
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize       = QSPI_ComConfig_ADSize_32bit;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins          = QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles  = DUMMY_CLOCK_CYCLES_READ_QUAD;
#endif /* STM32F412xG */
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode        = QSPI_ComConfig_FMode_Memory_Mapped;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode       = QSPI_ComConfig_ADMode_4Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode        = QSPI_ComConfig_IMode_1Line;
      QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode        = QSPI_ComConfig_DMode_4Line;
      QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);  
      
      GpioToggle();
      break;
            
    default :
      TransferStatus = FAILED; 
      break;
    }
  }
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static void QSPI_WriteEnable(void)
{
  /* Command Config for Write Enable Operation*/
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DHHC        = QSPI_ComConfig_DHHC_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DDRMode     = QSPI_ComConfig_DDRMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_NoData;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = WRITE_ENABLE_CMD ;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);
  
  /* Switch to Autopolling mode for the end of the Command */
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}
  QSPI_AutoPollingMode_SetInterval(0x10);
  QSPI_AutoPollingMode_Config(0x02, 0x02, QSPI_PMM_AND);
  QSPI_AutoPollingModeStopCmd(ENABLE);
  QSPI_SetDataLength(0x00);
  
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Auto_Polling;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = READ_STATUS_REG_CMD;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
  
  while(QSPI_GetFlagStatus(QSPI_FLAG_SM) == RESET)
  {}
  QSPI_ClearFlag(QSPI_FLAG_SM);
  QSPI_ClearFlag(QSPI_FLAG_TC);
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}  
}

/**
  * @brief  This function read the SR of the memory and wait the end of Command.
  * @param  None
  * @retval None
  */
static void QSPI_AutoPollingMemReady(void)
{
  /* Wait for end of Cmd */
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}
  QSPI_SetDataLength(0x00);
  QSPI_AutoPollingMode_Config(0x0000, 0x0101, QSPI_PMM_AND);
  QSPI_AutoPollingModeStopCmd(ENABLE);
  QSPI_SetDataLength(0);
  
  /* Reconfigure QSPI to automatic polling mode to wait for end of Command*/
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Auto_Polling;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = READ_STATUS_REG_CMD;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
  
  while(QSPI_GetFlagStatus(QSPI_FLAG_SM) == RESET)
  {}
  QSPI_ClearFlag(QSPI_FLAG_SM);  
  QSPI_ClearFlag(QSPI_FLAG_TC);
}

/**
  * @brief Configures the QSPI GPIO Pins.
  * @param None
  * @retval None
  */
static void QSPI_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the QSPI clock */
  QSPIx_CLK_INIT(QSPIx_CLK, ENABLE);
  
  /* Enable DMA1/2 AHB1 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(QSPIx_CLK_GPIO_CLK | QSPIx_CS_GPIO_CLK | QSPIx_D0_GPIO_CLK | \
                         QSPIx_D1_GPIO_CLK | QSPIx_D2_GPIO_CLK | QSPIx_D3_GPIO_CLK, ENABLE);
  
  /* QSPI GPIO Configuration -------------------------------------------------*/
  /* Connect QSPI pins to Alternate function */  
  GPIO_PinAFConfig(QSPIx_CLK_GPIO_PORT, QSPIx_CLK_SOURCE, QSPIx_CLK_AF);
  GPIO_PinAFConfig(QSPIx_CS_GPIO_PORT, QSPIx_CS_SOURCE, QSPIx_CS_AF);
  GPIO_PinAFConfig(QSPIx_D0_GPIO_PORT, QSPIx_D0_SOURCE, QSPIx_D0_AF);
  GPIO_PinAFConfig(QSPIx_D1_GPIO_PORT, QSPIx_D1_SOURCE, QSPIx_D1_AF);
  GPIO_PinAFConfig(QSPIx_D2_GPIO_PORT, QSPIx_D2_SOURCE, QSPIx_D2_AF);
  GPIO_PinAFConfig(QSPIx_D3_GPIO_PORT, QSPIx_D3_SOURCE, QSPIx_D3_AF);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  
  /* QSPI CLK pin configuration */
  GPIO_InitStructure.GPIO_Pin = QSPIx_CLK_PIN;
  GPIO_Init(QSPIx_CLK_GPIO_PORT, &GPIO_InitStructure);
  
  /* QSPI D0 pin configuration */
  GPIO_InitStructure.GPIO_Pin = QSPIx_D0_PIN;
  GPIO_Init(QSPIx_D0_GPIO_PORT, &GPIO_InitStructure);  
  
  /* QSPI D1 pin configuration */
  GPIO_InitStructure.GPIO_Pin = QSPIx_D1_PIN;
  GPIO_Init(QSPIx_D1_GPIO_PORT, &GPIO_InitStructure);
  
  /* QSPI D2 pin configuration */
  GPIO_InitStructure.GPIO_Pin = QSPIx_D2_PIN;
  GPIO_Init(QSPIx_D2_GPIO_PORT, &GPIO_InitStructure);
  
  /* QSPI D3 pin configuration */
  GPIO_InitStructure.GPIO_Pin = QSPIx_D3_PIN;
  GPIO_Init(QSPIx_D3_GPIO_PORT, &GPIO_InitStructure);
  
  /* QSPI CS pin configuration */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = QSPIx_CS_PIN;
  GPIO_Init(QSPIx_CS_GPIO_PORT, &GPIO_InitStructure);

#if defined(STM32F413_423xx)
  /* GPIOC Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  /* Configure PC13 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
#else
    /* GPIOC Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Configure PC13 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif /* STM32F413_423xx */
 }

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while(1)
  {
  }
}
#endif

/**
  * @brief  Toggle the GPIOs
  * @param  None
  * @retval None
  */
#if defined(__CC_ARM)
#pragma arm section code = ".qspi"
#pragma no_inline
static void GpioToggle(void)
#elif defined(__ICCARM__)
static void GpioToggle(void) @ ".qspi"
#elif defined(__GNUC__)
static void __attribute__((section(".qspi"), noinline)) GpioToggle(void)
#endif
{ 
 __IO uint32_t counterdelay = 0x00; 
  while (1)
  {
#if defined(STM32F413_423xx)   
    /* Set PG13 */
    GPIOG->BSRRL =  GPIO_Pin_13;
    for(counterdelay = 0; counterdelay < 0xFF; counterdelay++);
    /* Reset PG13 */
    GPIOG->BSRRH =  GPIO_Pin_13;
    for(__IO uint32_t counterdelay = 0; counterdelay < 0xFF; counterdelay++);
#else
    /* Set PC13 */
    GPIOC->BSRRL =  GPIO_Pin_13;
    for(counterdelay = 0; counterdelay < 0xFF; counterdelay++);
    /* Reset PC13 */
    GPIOC->BSRRH =  GPIO_Pin_13;
    for(__IO uint32_t counterdelay = 0; counterdelay < 0xFF; counterdelay++);   
#endif /* STM32F413_423xx */    
  }
}
#if defined(__CC_ARM)
#pragma arm section code
#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
