/*
 * port from "fire stm32f4 challenger board" bsp
 */
#include "hal_sdram.h"   

/**
  * @brief  延迟一段时间
  * @param  延迟的时间长度
  * @retval None
  */
static void SDRAM_delay(volatile uint32_t nCount)
{
    volatile uint32_t index = 0; 
    for(index = (100000 * nCount); index != 0; index--)
    {
    }
}

/**
  * @brief  初始化控制SDRAM的IO
  * @param  无
  * @retval 无
  */
static void SDRAM_GPIO_Config(void)
{		
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能SDRAM相关的GPIO时钟 */

                           /*地址信号线*/
    RCC_AHB1PeriphClockCmd(FMC_A0_GPIO_CLK | FMC_A1_GPIO_CLK | FMC_A2_GPIO_CLK | 
                           FMC_A3_GPIO_CLK | FMC_A4_GPIO_CLK | FMC_A5_GPIO_CLK |
                           FMC_A6_GPIO_CLK | FMC_A7_GPIO_CLK | FMC_A8_GPIO_CLK |
                           FMC_A9_GPIO_CLK | FMC_A10_GPIO_CLK| FMC_A11_GPIO_CLK|  
                           /*数据信号线*/
                           FMC_D0_GPIO_CLK | FMC_D1_GPIO_CLK | FMC_D2_GPIO_CLK | 
                           FMC_D3_GPIO_CLK | FMC_D4_GPIO_CLK | FMC_D5_GPIO_CLK |
                           FMC_D6_GPIO_CLK | FMC_D7_GPIO_CLK | FMC_D8_GPIO_CLK |
                           FMC_D9_GPIO_CLK | FMC_D10_GPIO_CLK| FMC_D11_GPIO_CLK|
                           FMC_D12_GPIO_CLK| FMC_D13_GPIO_CLK| FMC_D14_GPIO_CLK|
                           FMC_D15_GPIO_CLK|  
                           /*控制信号线*/
                           FMC_CS_GPIO_CLK | FMC_BA0_GPIO_CLK | FMC_BA1_GPIO_CLK | 
                           FMC_WE_GPIO_CLK | FMC_RAS_GPIO_CLK | FMC_CAS_GPIO_CLK |
                           FMC_CLK_GPIO_CLK | FMC_CKE_GPIO_CLK | FMC_UDQM_GPIO_CLK |
                           FMC_LDQM_GPIO_CLK, ENABLE);
    
    /*-- GPIO 配置 -----------------------------------------------------*/

    /* 通用 GPIO 配置 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //配置为复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   
    
    /*地址信号线 针对引脚配置*/
    GPIO_InitStructure.GPIO_Pin = FMC_A0_GPIO_PIN; 
    GPIO_Init(FMC_A0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A0_GPIO_PORT, FMC_A0_PINSOURCE , FMC_A0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A1_GPIO_PIN; 
    GPIO_Init(FMC_A1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A1_GPIO_PORT, FMC_A1_PINSOURCE , FMC_A1_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A2_GPIO_PIN; 
    GPIO_Init(FMC_A2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A2_GPIO_PORT, FMC_A2_PINSOURCE , FMC_A2_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A3_GPIO_PIN; 
    GPIO_Init(FMC_A3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A3_GPIO_PORT, FMC_A3_PINSOURCE , FMC_A3_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A4_GPIO_PIN; 
    GPIO_Init(FMC_A4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A4_GPIO_PORT, FMC_A4_PINSOURCE , FMC_A4_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A5_GPIO_PIN; 
    GPIO_Init(FMC_A5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A5_GPIO_PORT, FMC_A5_PINSOURCE , FMC_A5_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A6_GPIO_PIN; 
    GPIO_Init(FMC_A6_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A6_GPIO_PORT, FMC_A6_PINSOURCE , FMC_A6_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A7_GPIO_PIN; 
    GPIO_Init(FMC_A7_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A7_GPIO_PORT, FMC_A7_PINSOURCE , FMC_A7_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A8_GPIO_PIN; 
    GPIO_Init(FMC_A8_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A8_GPIO_PORT, FMC_A8_PINSOURCE , FMC_A8_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A9_GPIO_PIN; 
    GPIO_Init(FMC_A9_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A9_GPIO_PORT, FMC_A9_PINSOURCE , FMC_A9_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A10_GPIO_PIN; 
    GPIO_Init(FMC_A10_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A10_GPIO_PORT, FMC_A10_PINSOURCE , FMC_A10_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_A11_GPIO_PIN; 
    GPIO_Init(FMC_A11_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_A11_GPIO_PORT, FMC_A11_PINSOURCE , FMC_A11_AF);
    
    /*数据信号线 针对引脚配置*/
    GPIO_InitStructure.GPIO_Pin = FMC_D0_GPIO_PIN; 
    GPIO_Init(FMC_D0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D0_GPIO_PORT, FMC_D0_PINSOURCE , FMC_D0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D1_GPIO_PIN; 
    GPIO_Init(FMC_D1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D1_GPIO_PORT, FMC_D1_PINSOURCE , FMC_D1_AF);
      
    GPIO_InitStructure.GPIO_Pin = FMC_D2_GPIO_PIN; 
    GPIO_Init(FMC_D2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D2_GPIO_PORT, FMC_D2_PINSOURCE , FMC_D2_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D3_GPIO_PIN; 
    GPIO_Init(FMC_D3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D3_GPIO_PORT, FMC_D3_PINSOURCE , FMC_D3_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D4_GPIO_PIN; 
    GPIO_Init(FMC_D4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D4_GPIO_PORT, FMC_D4_PINSOURCE , FMC_D4_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D5_GPIO_PIN; 
    GPIO_Init(FMC_D5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D5_GPIO_PORT, FMC_D5_PINSOURCE , FMC_D5_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D6_GPIO_PIN; 
    GPIO_Init(FMC_D6_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D6_GPIO_PORT, FMC_D6_PINSOURCE , FMC_D6_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D7_GPIO_PIN; 
    GPIO_Init(FMC_D7_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D7_GPIO_PORT, FMC_D7_PINSOURCE , FMC_D7_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D8_GPIO_PIN; 
    GPIO_Init(FMC_D8_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D8_GPIO_PORT, FMC_D8_PINSOURCE , FMC_D8_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D9_GPIO_PIN; 
    GPIO_Init(FMC_D9_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D9_GPIO_PORT, FMC_D9_PINSOURCE , FMC_D9_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D10_GPIO_PIN; 
    GPIO_Init(FMC_D10_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D10_GPIO_PORT, FMC_D10_PINSOURCE , FMC_D10_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D11_GPIO_PIN; 
    GPIO_Init(FMC_D11_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D11_GPIO_PORT, FMC_D11_PINSOURCE , FMC_D11_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D12_GPIO_PIN; 
    GPIO_Init(FMC_D12_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D12_GPIO_PORT, FMC_D12_PINSOURCE , FMC_D12_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D13_GPIO_PIN; 
    GPIO_Init(FMC_D13_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D13_GPIO_PORT, FMC_D13_PINSOURCE , FMC_D13_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D14_GPIO_PIN; 
    GPIO_Init(FMC_D14_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D14_GPIO_PORT, FMC_D14_PINSOURCE , FMC_D14_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_D15_GPIO_PIN; 
    GPIO_Init(FMC_D15_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_D15_GPIO_PORT, FMC_D15_PINSOURCE , FMC_D15_AF);
    
    /*控制信号线*/
    GPIO_InitStructure.GPIO_Pin = FMC_CS_GPIO_PIN; 
    GPIO_Init(FMC_CS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_CS_GPIO_PORT, FMC_CS_PINSOURCE , FMC_CS_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_BA0_GPIO_PIN; 
    GPIO_Init(FMC_BA0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_BA0_GPIO_PORT, FMC_BA0_PINSOURCE , FMC_BA0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_BA1_GPIO_PIN;
    GPIO_Init(FMC_BA1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_BA1_GPIO_PORT, FMC_BA1_PINSOURCE , FMC_BA1_AF);
      
    GPIO_InitStructure.GPIO_Pin = FMC_WE_GPIO_PIN; 
    GPIO_Init(FMC_WE_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_WE_GPIO_PORT, FMC_WE_PINSOURCE , FMC_WE_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_RAS_GPIO_PIN; 
    GPIO_Init(FMC_RAS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_RAS_GPIO_PORT, FMC_RAS_PINSOURCE , FMC_RAS_AF);
      
    GPIO_InitStructure.GPIO_Pin = FMC_CAS_GPIO_PIN; 
    GPIO_Init(FMC_CAS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_CAS_GPIO_PORT, FMC_CAS_PINSOURCE , FMC_CAS_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_CLK_GPIO_PIN; 
    GPIO_Init(FMC_CLK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_CLK_GPIO_PORT, FMC_CLK_PINSOURCE , FMC_CLK_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_CKE_GPIO_PIN; 
    GPIO_Init(FMC_CKE_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_CKE_GPIO_PORT, FMC_CKE_PINSOURCE , FMC_CKE_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_UDQM_GPIO_PIN; 
    GPIO_Init(FMC_UDQM_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_UDQM_GPIO_PORT, FMC_UDQM_PINSOURCE , FMC_UDQM_AF);
    
    GPIO_InitStructure.GPIO_Pin = FMC_LDQM_GPIO_PIN; 
    GPIO_Init(FMC_LDQM_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FMC_LDQM_GPIO_PORT, FMC_LDQM_PINSOURCE , FMC_LDQM_AF);
}

/**
  * @brief  对SDRAM芯片进行初始化配置
  * @param  None. 
  * @retval None.
  */
static void SDRAM_InitSequence(void)
{
    FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
    uint32_t tmpr = 0;
  
/* Step 3 --------------------------------------------------------------------*/
    /* 配置命令：开启提供给SDRAM的时钟 */
    FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
    FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
    /* 检查SDRAM标志，等待至SDRAM空闲 */ 
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
    /* 发送上述命令*/
    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
  
/* Step 4 --------------------------------------------------------------------*/
    /* 约100ms的延时 */
    SDRAM_delay(10);
    
/* Step 5 --------------------------------------------------------------------*/
    /* 配置命令：对所有的bank预充电 */ 
    FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
    FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
    /* 检查SDRAM标志，等待至SDRAM空闲 */ 
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
    /* 发送上述命令*/
    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 6 --------------------------------------------------------------------*/
    /* 配置命令：自动刷新 */   
    FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
    FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
    /* 检查SDRAM标志，等待至SDRAM空闲 */ 
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
    /* 发送第1次自动刷新命令*/
    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
    
    /* 检查SDRAM标志，等待至SDRAM空闲 */ 
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
    /* 发送第2次自动刷新命令*/
    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 7 --------------------------------------------------------------------*/
    /* 设置sdram寄存器配置 */
    tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    
    /* 配置命令：设置SDRAM寄存器 */
    FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
    FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
    FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
    FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
    /* 检查SDRAM标志，等待至SDRAM空闲 */ 
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
    /* 发送上述命令*/
    FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 8 --------------------------------------------------------------------*/

    /* 设置刷新计数器 */
    /* (15.62 us x Freq) - 20 */
    FMC_SetRefreshCount(1386);
    /* 发送上述命令*/
    while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
    {
    }
}


/**
  * @brief  初始化配置使用SDRAM的FMC及GPIO接口，
  *         本函数在SDRAM读写操作前需要被调用
  * @param  None
  * @retval None
  */
void SDRAM_Init(void)
{
    FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
    FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 
    
    /* 配置FMC接口相关的 GPIO*/
    SDRAM_GPIO_Config();
    
    /* 使能 FMC 时钟 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
 
    /* 配置 FMC 相关参数 ---------------------------------------------------------*/
    /* SDCLK： 90 Mhz (HCLK/2 :180Mhz/2) */
    /* TMRD: 2 Clock cycles */
    FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;      
    /* TXSR: min=70ns (7x11.11ns) */
    FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
    /* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
    FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;
    /* TRC:  min=70 (7x11.11ns) */        
    FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;         
    /* TWR:  min=1+ 7ns (1+1x11.11ns) */
    FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;      
    /* TRP:  20ns => 2x11.11ns */
    FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;                
    /* TRCD: 20ns => 2x11.11ns */
    FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;

    /* FMC SDRAM 控制配置 */
    FMC_SDRAMInitStructure.FMC_Bank = FMC_BANK_SDRAM;
    /* 行地址线宽度: [7:0] */
    FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
    /* 列地址线宽度: [11:0] */
    FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
    /* 数据线宽度 */
    FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = SDRAM_MEMORY_WIDTH;
    /* SDRAM内部bank数量*/
    FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
    /* CAS潜伏期 */
    FMC_SDRAMInitStructure.FMC_CASLatency = SDRAM_CAS_LATENCY; 
    /* 禁止写保护*/
    FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
    /* SDCLK时钟分频因子，SDCLK = HCLK/SDCLOCK_PERIOD*/
    FMC_SDRAMInitStructure.FMC_SDClockPeriod = SDCLOCK_PERIOD; 
    /* 突发读模式设置*/  
    FMC_SDRAMInitStructure.FMC_ReadBurst = SDRAM_READBURST;
    /* 读延迟配置 */
    FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
    /* SDRAM时序参数 */
    FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
    
    /* FMC SDRAM bank initialization */
    FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
    
    /* FMC SDRAM device initialization sequence */
    SDRAM_InitSequence(); 
  
}


void hal_sdram_init(void)
{
    SDRAM_Init();
}

