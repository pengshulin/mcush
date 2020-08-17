/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <string.h>

#if HAL_CAN

/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PB8 ------> CAN_RX
   PB9 ------> CAN_TX
   ----------------------------- 
 */

#ifndef HAL_CAN_DEFINE 
    #define HAL_CAN_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
    #define HAL_CAN_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO
    #define HAL_CAN_RCC_CAN_ENABLE_CMD     RCC_APB1PeriphClockCmd
    #define HAL_CAN_RCC_CAN_ENABLE_BIT     RCC_APB1Periph_CAN1
    #define HAL_CANx                       CAN1
    #define HAL_CAN_RX_PORT                GPIOB
    #define HAL_CAN_RX_PIN                 GPIO_Pin_8
    #define HAL_CAN_TX_PORT                GPIOB
    #define HAL_CAN_TX_PIN                 GPIO_Pin_9
    #define HAL_CAN_PIN_REMAP              GPIO_Remap1_CAN1
    #define HAL_CAN_RX_IRQn                CAN1_RX1_IRQn
    #define HAL_CAN_RX_IRQHandler          CAN1_RX1_IRQHandler
    #define HAL_CAN_TX_IRQn                USB_HP_CAN1_TX_IRQn
    #define HAL_CAN_TX_IRQHandler          USB_HP_CAN1_TX_IRQHandler
#endif

#ifndef HAL_CAN_BAUDRATE_DEF
#define HAL_CAN_BAUDRATE_DEF  1000000
#endif

extern const GPIO_TypeDef * const ports[];

os_queue_handle_t hal_can_queue_rx, hal_can_queue_tx;

static uint8_t _inited;
static uint32_t hal_can_tx_counter;
static uint32_t hal_can_rx_counter;
static uint32_t hal_can_baudrate=HAL_CAN_BAUDRATE_DEF;


/* configuration table */
typedef struct {
    uint32_t bps;
    uint32_t prescaler:10;  /* 1~1023 */
    uint32_t SWJ:2;  /* resynchronization jump width */
    uint32_t BS2:3;  /* time segment 2 */
    uint32_t BS1:4;  /* time segment 1 */
} can_baudrate_config_t;

#include "hal_can_baudrates.c.inc"


int hal_can_init( void )
{
    GPIO_InitTypeDef gpio_init;
    CAN_FilterInitTypeDef can_filter_init;
    NVIC_InitTypeDef nvic_init;
    
    if( _inited )
        return 1;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_QUEUE_BUFFER( can_rx, HAL_CAN_QUEUE_RX_LEN, sizeof(can_message_t) );
    DEFINE_STATIC_QUEUE_BUFFER( can_tx, HAL_CAN_QUEUE_TX_LEN, sizeof(can_message_t) );
    hal_can_queue_rx = os_queue_create_static( "canRxQ", HAL_CAN_QUEUE_RX_LEN, sizeof(can_message_t),
                                            &static_queue_buffer_can_rx );
    hal_can_queue_tx = os_queue_create_static( "canTxQ", HAL_CAN_QUEUE_TX_LEN, sizeof(can_message_t),
                                            &static_queue_buffer_can_tx );
#else
    hal_can_queue_rx = os_queue_create( "canRxQ", HAL_CAN_QUEUE_RX_LEN, sizeof(can_message_t) );
    hal_can_queue_tx = os_queue_create( "canTxQ", HAL_CAN_QUEUE_TX_LEN, sizeof(can_message_t) );
#endif
    if( (hal_can_queue_rx == NULL) || (hal_can_queue_tx == NULL) )
        return 0;

    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

    // clock
    HAL_CAN_RCC_GPIO_ENABLE_CMD( HAL_CAN_RCC_GPIO_ENABLE_BIT, ENABLE );
    HAL_CAN_RCC_CAN_ENABLE_CMD( HAL_CAN_RCC_CAN_ENABLE_BIT, ENABLE );

#if defined(HAL_CAN_PIN_REMAP)
    GPIO_PinRemapConfig( HAL_CAN_PIN_REMAP, ENABLE );
#endif

    // alternative output port
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    gpio_init.GPIO_Pin = HAL_CAN_RX_PIN;
    GPIO_Init( HAL_CAN_RX_PORT, &gpio_init );
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Pin = HAL_CAN_TX_PIN;
    GPIO_Init( HAL_CAN_TX_PORT, &gpio_init );

    /* CAN register init */
    CAN_DeInit( HAL_CANx );
   
    /* CAN cell init */
    hal_can_set_baudrate(hal_can_baudrate);
  
    /* CAN filter init */
    can_filter_init.CAN_FilterNumber = 0;
    can_filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
    can_filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
    can_filter_init.CAN_FilterIdHigh = 0;
    can_filter_init.CAN_FilterIdLow = 0;
    can_filter_init.CAN_FilterMaskIdHigh = 0;
    can_filter_init.CAN_FilterMaskIdLow = 0;
    can_filter_init.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
    can_filter_init.CAN_FilterActivation = ENABLE;
    CAN_FilterInit( &can_filter_init );

    CAN_ITConfig( HAL_CANx, CAN_IT_FMP1, ENABLE );  /* Receive messages pending */
    CAN_ITConfig( HAL_CANx, CAN_IT_TME, ENABLE );   /* Transmit mailbox empty */
 
    /* Interrupt Enable */  
    nvic_init.NVIC_IRQChannel = HAL_CAN_RX_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &nvic_init );
    nvic_init.NVIC_IRQChannel = HAL_CAN_TX_IRQn;
    NVIC_Init( &nvic_init );

    CAN_OperatingModeRequest( HAL_CANx, CAN_OperatingMode_Normal );

    _inited = 1;
    return 1;
}

 
void hal_can_deinit( void )
{
    GPIO_InitTypeDef gpio_init;

    if( _inited )
    {
        os_queue_delete( hal_can_queue_rx );
        os_queue_delete( hal_can_queue_tx );
        hal_can_queue_rx = 0;
        hal_can_queue_tx = 0;

        CAN_DeInit( HAL_CANx );
     
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Mode = GPIO_Mode_IPU;
        gpio_init.GPIO_Pin = HAL_CAN_RX_PIN;
        GPIO_Init( HAL_CAN_RX_PORT, &gpio_init );
        gpio_init.GPIO_Pin = HAL_CAN_TX_PIN;
        GPIO_Init( HAL_CAN_TX_PORT, &gpio_init );
   
        _inited = 0; 
    }
}


void hal_can_reset( void )
{
    CAN_ClearFlag( HAL_CANx, CAN_FLAG_LEC );
    hal_can_tx_counter = 0;
    hal_can_rx_counter = 0;
    CAN_OperatingModeRequest( HAL_CANx, CAN_OperatingMode_Normal );
}


int hal_can_set_baudrate( int baudrate )
{
    CAN_InitTypeDef can_init;
    unsigned int i;

    for( i=0; i<sizeof(baudrate_config)/sizeof(can_baudrate_config_t); i++ )
    {
        if( baudrate_config[i].bps == (uint32_t)baudrate )
        {
            CAN_ClearFlag( HAL_CANx, CAN_FLAG_LEC );
            can_init.CAN_Mode = CAN_Mode_Normal;  // normal mode
            can_init.CAN_Prescaler = baudrate_config[i].prescaler;
            can_init.CAN_SJW = baudrate_config[i].SWJ;
            can_init.CAN_BS1 = baudrate_config[i].BS1;
            can_init.CAN_BS2 = baudrate_config[i].BS2;
            can_init.CAN_TTCM = DISABLE;  // time triggered communication mode
            can_init.CAN_ABOM = ENABLE;//DISABLE;  // automatic bus-off management
            can_init.CAN_AWUM = DISABLE;  // automatic wake-up mode
            can_init.CAN_NART = DISABLE;  // no automatic retransmission
            can_init.CAN_RFLM = DISABLE;  // receive fifo locked mode
            can_init.CAN_TXFP = DISABLE;  // transmit fifo priority
            CAN_Init( HAL_CANx, &can_init );  
            hal_can_baudrate = baudrate;
            if( _inited )
                CAN_OperatingModeRequest( HAL_CANx, CAN_OperatingMode_Normal );
            return 1;
        }
    }
    return 0;
}


int hal_can_get_baudrate( void )
{
    return hal_can_baudrate;
}


int hal_can_filter_set( int index, can_filter_t *filter, int enabled )
{
    CAN_FilterInitTypeDef can_filter_init;
    int f, m;

    /* CAN1/CAN2 shared filters */
#if HAL_CAN2
    if( (index < 0) || (index >= 14) )
#else
    if( (index < 0) || (index >= 28) )
#endif
        return 0;
    can_filter_init.CAN_FilterNumber = (uint8_t)index;
    can_filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
    can_filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
    if( filter->ext )
    {
        f = ((filter->ext_id & 0x1FFFFFFF)<<3) | 4;
        m = ((filter->ext_id_mask & 0x1FFFFFFF)<<3) | 4;
    }
    else
    {
        f = (filter->std_id & 0x7FF)<<21;
        m = ((filter->std_id_mask & 0x7FF)<<21) | 4;
    }
    if( filter->remote )
    {
        f |= 2;
        m |= 2;
    }        
    can_filter_init.CAN_FilterIdHigh = (f>>16) & 0xFFFF;
    can_filter_init.CAN_FilterIdLow = f & 0xFFFF;
    can_filter_init.CAN_FilterMaskIdHigh = (m>>16) & 0xFFFF;
    can_filter_init.CAN_FilterMaskIdLow = m & 0xFFFF;
    can_filter_init.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
    can_filter_init.CAN_FilterActivation = enabled ? ENABLE : DISABLE;
    CAN_FilterInit( &can_filter_init );
    return 1;
}


int hal_can_filter_get( int index, can_filter_t *filter, int *enabled )
{
    int f, m;

    /* CAN1/CAN2 shared filters */
#if HAL_CAN2
    if( (index < 0) || (index >= 14) )
#else
    if( (index < 0) || (index >= 28) )
#endif
        return 0;

    if( CAN1->FA1R & (1<<index) )
    {
        *enabled = 1;
        m = CAN1->sFilterRegister[index].FR2;
        f = CAN1->sFilterRegister[index].FR1 & m; 
        filter->ext = (f & 4) ? 1 : 0;
        filter->remote = (f & 2) ? 1 : 0;
        if( filter->ext )
        {
            filter->ext_id = (f >> 3) & 0x1FFFFFFF;
            filter->ext_id_mask = (m >> 3) & 0x1FFFFFFF;
        }
        else
        {
            filter->std_id = (f >> 21) & 0x7FF;
            filter->std_id_mask = (m >> 21) & 0x7FF;
        }
    }
    else
        *enabled = 0;

    return 1;
}


int hal_can_get_error( void )
{
    return CAN_GetLastErrorCode(HAL_CANx);
}


int hal_can_send( can_message_t *msg )
{
    CanTxMsg txmsg;
    int i;

    if( msg->len > 8 )
        return -1;

    /* transmit */
    if( msg->ext )
        txmsg.ExtId = msg->id;  /* ignored for CAN_Id_Standard */
    else
        txmsg.StdId = msg->id;  /* ignored for CAN_Id_Extended */
    txmsg.RTR = msg->remote ? CAN_RTR_Remote : CAN_RTR_Data;
    txmsg.IDE = msg->ext ? CAN_Id_Extended : CAN_Id_Standard;
    txmsg.DLC = msg->len;
    for( i=0; i<msg->len; i++ )
        txmsg.Data[i] = msg->data[i];

    i = CAN_Transmit( HAL_CANx, &txmsg );
    return (i == CAN_TxStatus_NoMailBox) ? -1 : i;
}


void hal_can_cancel( int id )
{
    CAN_CancelTransmit( HAL_CANx, (uint8_t)id );
}


int hal_can_receive( can_message_t *msg )
{
    CanRxMsg rxmsg;

    if( CAN_MessagePending( HAL_CANx, CAN_FIFO1 ) == 0 )
        return 0;

    memset( &rxmsg, 0, sizeof(rxmsg) );
    CAN_Receive( HAL_CANx, CAN_FIFO1, &rxmsg );
    CAN_FIFORelease( HAL_CANx, CAN_FIFO1 );
    msg->ext = (rxmsg.IDE == CAN_Id_Extended) ? 1 : 0;
    if( msg->ext )
        msg->id = rxmsg.ExtId;
    else
        msg->id = rxmsg.StdId;
    msg->remote = (rxmsg.RTR == CAN_RTR_Remote) ? 1 : 0;
    msg->len = rxmsg.DLC;
    if( msg->len )
        memcpy( (void*)msg->data, rxmsg.Data, msg->len );
    return 1;
}


int hal_can_read( can_message_t *msg, int block_ticks )
{
    if( os_queue_get( hal_can_queue_rx, msg, block_ticks ) )
        return 1;
    else
        return 0;
}


int hal_can_write( can_message_t *msg )
{
    if( hal_can_send( msg ) >= 0 )
        return 1;
    /* mailbox busy, cache to queue */
    if( os_queue_put( hal_can_queue_tx, msg, 0 ) )
        return 1;
    else
        return 0;
}


int hal_can_get_counter( uint32_t *tx_counter, uint32_t *rx_counter )
{
    if( ! _inited )
        return 0;
    *tx_counter = hal_can_tx_counter; 
    *rx_counter = hal_can_rx_counter; 
    return 1;
}


void HAL_CAN_TX_IRQHandler(void)
{
    can_message_t msg;

    if( CAN_GetITStatus( HAL_CANx, CAN_IT_TME) == SET )
    {
        CAN_ClearITPendingBit( HAL_CANx, CAN_IT_TME );
        hal_can_tx_counter += 1;
        /* send remaining */ 
        while( os_queue_get_isr( hal_can_queue_tx, &msg ) )
        {
            if( hal_can_send( &msg ) < 0 )
                break;
            hal_can_tx_counter += 1;
        }
    }
}


void HAL_CAN_RX_IRQHandler(void)
{
    can_message_t msg;

    while( hal_can_receive( &msg ) )
    {
        hal_can_rx_counter += 1;
        if( os_queue_put_isr( hal_can_queue_rx, &msg ) == 0 )
            break;
    }
}



#endif

