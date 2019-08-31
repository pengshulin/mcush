#include "stm32f4x7_eth.h"
#include "lwip/opt.h"
#include "LAN8742A.h"
#include "lwip/netif.h"
#include "lwip_config.h"
#include "lwip/dhcp.h"
#include "mcush.h"
#include "FreeRTOS.h"
#include "task_dhcpc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ETH_InitTypeDef ETH_InitStructure;
__IO uint32_t EthStatus = 0;
extern struct netif gnetif;


/* Private function prototypes -----------------------------------------------*/
static void ETH_GPIO_Config(void);
static void ETH_MACDMA_Config(void);




const int8_t reg_id[] = {0, 1, 2, 3, 4, 5, 6, 17, 18, 26, 27, 29, 30, 31, -1};
const char * const reg_name[] = {"BCR", "BSR", "IDN1", "IDN2", "NADV", "NLPA", "NEXP", "MODE", "SMOD", "SEC", "SCSI", "INTS", "INTM", "SCS"};


static void print_reg( const char *name, int value )
{
    int name_len = strlen(name);
    shell_printf("%s:", name);
    while( name_len++ < 5 )
        shell_write_char(' '); 
    shell_printf("%04X\n", value);
}


void print_all_regs(void)
{
    int i=0;
    while( reg_id[i] != -1 )
    {
        print_reg(reg_name[i], ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, reg_id[i]));
        i++;
    }
}


int print_reg_by_id(int idx)
{
    int i=0;
    while( reg_id[i] != -1 )
    {
        if( reg_id[i] == idx )
        {
            print_reg(reg_name[i], ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, reg_id[i]));
            return 1;
        }
        i++;
    }
    return 0;
}


int print_reg_by_name(const char *name)
{
    int i=0;
    while( reg_id[i] != -1 )
    {
        if( strcasecmp( reg_name[i], name ) == 0  )
        {
            print_reg(name, ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, reg_id[i]));
            return 1;
        }
        i++;
    } 
    return 0;
}


int set_reg_by_name(const char *name, int value)
{
    int i=0;
    while( reg_id[i] != -1 )
    {
        if( strcasecmp( reg_name[i], name ) == 0  )
        {
            ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, reg_id[i], value);
            return 1;
        }
        i++;
    } 
    return 0;
}


/* debug command */
int cmd_lan8720( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', shell_str_command, shell_str_command, "info|reset|read|write|down|loop" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'n', shell_str_name, shell_str_name, "name param" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'v', shell_str_value, shell_str_value, "value param" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    const char *cmd=0;
    const char *name=0;
    int value, reg, reg2;
    uint8_t name_set=0, value_set=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_command ) == 0 )
                cmd = opt.value;
            else if( STRCMP( opt.spec->name, shell_str_name ) == 0 )
            {
                name = opt.value;
                if( *name )
                    name_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
            {
                if( parse_int( opt.value, &value ) == 0 )
                {
                    shell_write_line("val err");
                    return 1;
                }
                value_set = 1;
            } 
            else
                STOP_AT_INVALID_ARGUMENT 
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( cmd==NULL || strcmp(cmd, shell_str_info) == 0 )
    {
        print_all_regs();
    }
    else if( strcmp( cmd, shell_str_reset ) == 0 )
    {
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_Reset);
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);
    }
    else if( strcmp( cmd, "down" ) == 0 )
    {
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_Powerdown);
    }
    else if( strcmp( cmd, "loop" ) == 0 )
    {
        /* lan8720 -c loop        --> near loop
           lan8720 -c loop -v 1   --> near loop
           lan8720 -c loop -v 2   --> far loop
           lan8720 -c loop -v 0   --> back normal
         */
        reg = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, 0);
        reg2 = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, 17);
        if( value_set && (value==2) )  /* far loopback mode */
        {
            reg &= ~0x4000;
            reg2 |= 0x0200;
        }
        else if( value_set && (value==0) )  /* back to normal */
        {
            reg &= ~0x4000;
            reg2 &= ~0x0200;
        }
        else  /* near loopback mode */
        {
            reg |= 0x4000;
            reg2 &= ~0x0200;
        }
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, 0, reg);
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, 17, reg2);
    }
    else if( strcmp( cmd, shell_str_read ) == 0 )
    {
        if( name_set )
            return print_reg_by_name( name ) ? 0 : 1;
        else
            return -1;
    }
    else if( strcmp( cmd, shell_str_write ) == 0 )
    {
        if( name_set && value_set )
            return set_reg_by_name( name, value ) ? 0 : 1;
        else
            return -1;
    }
    else
        return -1;
    return 0;
}

const shell_cmd_t cmd_tab_lan8720[] = {
{   0, 0, "lan8720",  cmd_lan8720, 
    "debug phy chip",
    "lan8720"  },
{   CMD_END  } };




/**
  * @brief  ETH_BSP_Config
  * @param  None
  * @retval None
  */
void ETH_BSP_Config(void)
{
    /* Configure the GPIO ports for ethernet pins */
    ETH_GPIO_Config();

    /* Configure the Ethernet MAC/DMA */
    ETH_MACDMA_Config();

    /* Get Ethernet link status*/
    if((ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_Linked_Status))
    {
        EthStatus |= ETH_LINK_FLAG;
    }
    
    shell_add_cmd_table( cmd_tab_lan8720 );
}


/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
static void ETH_MACDMA_Config(void)
{
    /* Enable ETHERNET clock  */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                           RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    while (ETH_GetSoftwareResetStatus() == SET)
    {
        vTaskDelay(1);
    }

    /* ETHERNET Configuration --------------------------------------------------*/
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
    /* 开启网络自适应功能 */
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
    //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
    //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    /* 关闭反馈 */
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    /* 关闭重传功能 */
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    /* 关闭自动去除PDA/CRC功能  */
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    /* 关闭接收所有的帧 */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    /* 允许接收所有广播帧 */
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    /* 关闭混合模式的地址过滤  */
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    /* 对于组播地址使用完美地址过滤    */
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    /* 对单播地址使用完美地址过滤  */
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    /* 开启ipv4和TCP/UDP/ICMP的帧校验和卸载   */
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

    /*------------------------   DMA   -----------------------------------*/

    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    /*当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
    这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧*/

    /* 开启丢弃TCP/IP错误帧 */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    /* 开启接收数据的存储转发模式  */
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    /* 开启发送数据的存储转发模式   */
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

    /* 禁止转发错误帧 */
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    /* 不转发过小的好帧 */
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    /* 打开处理第二帧功能 */
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    /* 开启DMA传输的地址对齐功能 */
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
    /* 开启固定突发功能 */
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    /* DMA发送的最大突发长度为32个节拍 */
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    /* DMA接收的最大突发长度为32个节拍 */
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
    /* 配置ETH */
    EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
    ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);

    /* reset */
    ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_Reset);
    ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ETH_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOs clocks */
    RCC_AHB1PeriphClockCmd(ETH_MDIO_GPIO_CLK            | ETH_MDC_GPIO_CLK          |
                           ETH_RMII_REF_CLK_GPIO_CLK    | ETH_RMII_CRS_DV_GPIO_CLK  |
                           ETH_RMII_RXD0_GPIO_CLK       | ETH_RMII_RXD1_GPIO_CLK    |
                           ETH_RMII_TX_EN_GPIO_CLK      | ETH_RMII_TXD0_GPIO_CLK    |
                           ETH_RMII_TXD1_GPIO_CLK       | ETH_NRST_GPIO_CLK         , ENABLE);

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


    /* MII/RMII Media interface selection --------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM324xG-EVAL  */
#ifdef PHY_CLOCK_MCO

    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
#endif /* PHY_CLOCK_MCO */

    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#elif defined RMII_MODE  /* Mode RMII with STM324xG-EVAL */
    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif



    /* Ethernet pins configuration ************************************************/

    GPIO_InitStructure.GPIO_Pin = ETH_NRST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(ETH_NRST_PORT, &GPIO_InitStructure);

    ETH_NRST_PIN_LOW();
    _eth_delay_(LAN8742A_RESET_DELAY);
    ETH_NRST_PIN_HIGH();
    _eth_delay_(LAN8742A_RESET_DELAY);

    /* Configure ETH_MDIO */
    GPIO_InitStructure.GPIO_Pin = ETH_MDIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(ETH_MDIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_MDIO_PORT, ETH_MDIO_SOURCE, ETH_MDIO_AF);

    /* Configure ETH_MDC */
    GPIO_InitStructure.GPIO_Pin = ETH_MDC_PIN;
    GPIO_Init(ETH_MDC_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_MDC_PORT, ETH_MDC_SOURCE, ETH_MDC_AF);

    /* Configure ETH_RMII_REF_CLK */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_REF_CLK_PIN;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(ETH_RMII_REF_CLK_PORT, &GPIO_InitStructure);
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_PinAFConfig(ETH_RMII_REF_CLK_PORT, ETH_RMII_REF_CLK_SOURCE, ETH_RMII_REF_CLK_AF);

    /* Configure ETH_RMII_CRS_DV */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_CRS_DV_PIN;
    GPIO_Init(ETH_RMII_CRS_DV_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_CRS_DV_PORT, ETH_RMII_CRS_DV_SOURCE, ETH_RMII_CRS_DV_AF);

    /* Configure ETH_RMII_RXD0 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD0_PIN;
    GPIO_Init(ETH_RMII_RXD0_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_RXD0_PORT, ETH_RMII_RXD0_SOURCE, ETH_RMII_RXD0_AF);

    /* Configure ETH_RMII_RXD1 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD1_PIN;
    GPIO_Init(ETH_RMII_RXD1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_RXD1_PORT, ETH_RMII_RXD1_SOURCE, ETH_RMII_RXD1_AF);

    /* Configure ETH_RMII_TX_EN */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TX_EN_PIN;
    GPIO_Init(ETH_RMII_TX_EN_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TX_EN_PORT, ETH_RMII_TX_EN_SOURCE, ETH_RMII_TX_EN_AF);

    /* Configure ETH_RMII_TXD0 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD0_PIN;
    GPIO_Init(ETH_RMII_TXD0_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TXD0_PORT, ETH_RMII_TXD0_SOURCE, ETH_RMII_TXD0_AF);

    /* Configure ETH_RMII_TXD1 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD1_PIN;
    GPIO_Init(ETH_RMII_TXD1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TXD1_PORT, ETH_RMII_TXD1_SOURCE, ETH_RMII_TXD1_AF);
}


#define ETH_CHECK_LINK_RETRY  3
/* check ethernet link status every second */
void ETH_CheckLinkStatus(uint16_t PHYAddress)
{
    static uint8_t check_link_retry = 0;

    if( ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_Linked_Status )
    {
        check_link_retry = ETH_CHECK_LINK_RETRY;
        if( ! (EthStatus & ETH_LINK_FLAG)  )
        {
            netif_set_link_up( &gnetif );  /* set link up */
            EthStatus |= ETH_LINK_FLAG;
        }
    }
    else if( EthStatus & ETH_LINK_FLAG )
    {
        if( check_link_retry == 0 )
        {
            netif_set_link_down( &gnetif );  /* set link down */
            EthStatus &= ~ETH_LINK_FLAG;
            check_link_retry = ETH_CHECK_LINK_RETRY;
        }
        else
            check_link_retry--;
    }
    else
    {
        if( check_link_retry == 0 )
        {
            ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);
            check_link_retry = ETH_CHECK_LINK_RETRY;
        }
        else
            check_link_retry--;
    }
}

/**
  * @brief  Link callback function, this function is called on change of link status.
  * @param  The network interface
  * @retval None
  */
void ETH_link_callback(struct netif *netif)
{
    __IO uint32_t timeout = 0;
    uint32_t tmpreg, RegValue;

    if(netif_is_link_up(netif))
    {
        /* Restart the autonegotiation */
        if(ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
        {
            /* Reset Timeout counter */
            timeout = 0;

            /* Enable auto-negotiation */
            ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);

            /* Wait until the auto-negotiation will be completed */
            do
            {
                timeout++;
            } while (!(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));

            /* Reset Timeout counter */
            timeout = 0;

            /* Read the result of the auto-negotiation */
            RegValue = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_SPECIAL_CTRL_STA);

            /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
            if((RegValue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
            {
                /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
                ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
            }
            else
            {
                /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
                ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
            }
            /* Configure the MAC with the speed fixed by the auto-negotiation process */
            if(RegValue & PHY_SPEED_STATUS)
            {
                /* Set Ethernet speed to 10M following the auto-negotiation */
                ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
            }
            else
            {
                /* Set Ethernet speed to 100M following the auto-negotiation */
                ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
            }

//    	/* This is different for every PHY */
//			ETH_EXTERN_GetSpeedAndDuplex(ETHERNET_PHY_ADDRESS, &ETH_InitStructure);

            /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
            /* Get the ETHERNET MACCR value */
            tmpreg = ETH->MACCR;

            /* Set the FES bit according to ETH_Speed value */
            /* Set the DM bit according to ETH_Mode value */
            tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);

            /* Write to ETHERNET MACCR */
            ETH->MACCR = (uint32_t)tmpreg;

            _eth_delay_(ETH_REG_WRITE_DELAY);
            tmpreg = ETH->MACCR;
            ETH->MACCR = tmpreg;
        }

        /* Restart MAC interface */
        ETH_Start();

        send_dhcpc_event( DHCPC_EVENT_NETIF_UP );

        //netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);

        /* When the netif is fully configured this function must be called.*/
        netif_set_up(&gnetif);
    }
    else
    {
        ETH_Stop();

        send_dhcpc_event( DHCPC_EVENT_NETIF_DOWN );

        /* When the netif link is down this function must be called.*/
        netif_set_down(&gnetif);
    }
}

void ETH_EXTERN_GetSpeedAndDuplex(uint32_t PHYAddress, ETH_InitTypeDef* ETH_InitStruct)
{
    uint32_t RegValue;

    /* LAN8720A */
    /* Read status register, register number 31 = 0x1F */
    RegValue = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, 0x1F);
    /* Mask out bits which are not for speed and link indication, bits 4:2 are used */
    RegValue = (RegValue >> 2) & 0x07;

    /* Switch statement */
    switch (RegValue) {
    case 1: /* Base 10, half-duplex */
        ETH_InitStruct->ETH_Speed = ETH_Speed_10M;
        ETH_InitStruct->ETH_Mode = ETH_Mode_HalfDuplex;
        break;
    case 2: /* Base 100, half-duplex */
        ETH_InitStruct->ETH_Speed = ETH_Speed_100M;
        ETH_InitStruct->ETH_Mode = ETH_Mode_HalfDuplex;
        break;
    case 5: /* Base 10, full-duplex */
        ETH_InitStruct->ETH_Speed = ETH_Speed_10M;
        ETH_InitStruct->ETH_Mode = ETH_Mode_FullDuplex;
        break;
    case 6: /* Base 100, full-duplex */
        ETH_InitStruct->ETH_Speed = ETH_Speed_100M;
        ETH_InitStruct->ETH_Mode = ETH_Mode_FullDuplex;
        break;
    default:
        break;
    }
    /* LAN8720A */
}

