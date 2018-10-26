/**
  ******************************************************************************
  * @file    stm32f4x7_eth_conf_template.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Configuration file for the STM32F4x7xx Ethernet driver.
  *          This file should be copied to the application folder and renamed to
  *          stm32f4x7_eth_conf.h    
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F429_ETH_CONF_H
#define __STM32F429_ETH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Uncomment the line below when using time stamping and/or IPv4 checksum offload */
#define USE_ENHANCED_DMA_DESCRIPTORS

/* Uncomment the line below if you want to use user defined Delay function
   (for precise timing), otherwise default _eth_delay_ function defined within
   the Ethernet driver is used (less precise timing) */
#define USE_Delay

#ifdef USE_Delay
    #include "hal.h"
    #define _eth_delay_    hal_delay_10ms     /* User can provide more timing precise _eth_delay_ function
                                      ex. use Systick with time base of 10 ms (as done in the provided 
                                      STM32F4x7xx demonstrations) */
#else
  extern void ETH_Delay(__IO uint32_t nCount);
  #define _eth_delay_    ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif

/* Uncomment the line below to allow custom configuration of the Ethernet driver buffers */    
//#define CUSTOM_DRIVER_BUFFERS_CONFIG   

#ifdef  CUSTOM_DRIVER_BUFFERS_CONFIG
/* Redefinition of the Ethernet driver buffers size and count */   
 #define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for receive */
 #define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for transmit */
 #define ETH_RXBUFNB        20                  /* 20 Rx buffers of size ETH_RX_BUF_SIZE */
 #define ETH_TXBUFNB        5                   /* 5  Tx buffers of size ETH_TX_BUF_SIZE */
#endif


/* PHY configuration section **************************************************/
#ifdef USE_Delay
/* PHY Reset delay */ 
#define PHY_RESET_DELAY         ((uint32_t)0x000000FF)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY        ((uint32_t)0x00000FFF)
/* Delay when writing to Ethernet registers*/
#define ETH_REG_WRITE_DELAY     ((uint32_t)0x00000001)
/* LAN8742A Reset delay */ 
#define LAN8742A_RESET_DELAY    ((uint32_t)0x00000005)
#else
/* PHY Reset delay */ 
#define PHY_RESET_DELAY         ((uint32_t)0x000FFFFF)
/* PHY Configuration delay */ 
#define PHY_CONFIG_DELAY        ((uint32_t)0x00FFFFFF)
/* Delay when writing to Ethernet registers*/
#define ETH_REG_WRITE_DELAY     ((uint32_t)0x0000FFFF)
/* LAN8742A Reset delay */ 
#define LAN8742A_RESET_DELAY    ((uint32_t)0x00FFFFFF)
#endif

/*******************  PHY Extended Registers section : ************************/

/* These values are relatives to LAN8742A PHY and change from PHY to another,
   so the user have to update this value depending on the used external PHY */   

/* The LAN8742A PHY status register  */
#define PHY_IDN1                     ((uint16_t)2)
#define PHY_IDN2                     ((uint16_t)3)
#define PHY_NEGO_ADV                 ((uint16_t)4)
#define PHY_NEGO_LPA                 ((uint16_t)5)
#define PHY_NEGO_EXP                 ((uint16_t)6)
#define PHY_MODE                     ((uint16_t)17)
#define PHY_SPECIAL_MODES            ((uint16_t)18)
#define PHY_SYMBOL_ERR_COUNTER       ((uint16_t)26)
#define PHY_SPECIAL_CTRL_STA_IND     ((uint16_t)27)
#define PHY_INT_SRC                  ((uint16_t)29)
#define PHY_INT_MASK                 ((uint16_t)30)
#define PHY_SPECIAL_CTRL_STA         ((uint16_t)31)

#define PHY_SPEED_STATUS       ((uint16_t)0x0004) /* PHY Speed mask  1:10Mb/s       0:100Mb/s*/
#define PHY_DUPLEX_STATUS      ((uint16_t)0x0010) /* PHY Duplex mask 1:Full duplex  0:Half duplex*/


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F429_ETH_CONF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
