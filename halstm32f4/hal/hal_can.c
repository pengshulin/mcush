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
    #define HAL_CAN_RCC_GPIO_ENABLE_CMD    LL_AHB1_GRP1_EnableClock
    #define HAL_CAN_RCC_GPIO_ENABLE_BIT    LL_AHB1_GRP1_PERIPH_GPIOB
    #define HAL_CAN_RCC_CAN_ENABLE_CMD     LL_APB1_GRP1_EnableClock
    #define HAL_CAN_RCC_CAN_ENABLE_BIT     LL_APB1_GRP1_PERIPH_CAN1
    #define HAL_CANx                       CAN1
    #define HAL_CAN_RX_PORT                GPIOB
    #define HAL_CAN_RX_PIN                 GPIO_PIN_8
    #define HAL_CAN_RX_AF                  LL_GPIO_AF_9
    #define HAL_CAN_TX_PORT                GPIOB
    #define HAL_CAN_TX_PIN                 GPIO_PIN_9
    #define HAL_CAN_TX_AF                  LL_GPIO_AF_9
    #define HAL_CAN_RX_IRQn                CAN1_RX1_IRQn
    #define HAL_CAN_RX_IRQHandler          CAN1_RX1_IRQHandler
    #define HAL_CAN_TX_IRQn                CAN1_TX_IRQn
    #define HAL_CAN_TX_IRQHandler          CAN1_TX_IRQHandler
#endif

#ifndef HAL_CAN_BAUDRATE_DEF
#define HAL_CAN_BAUDRATE_DEF  1000000
#endif


QueueHandle_t hal_can_queue_rx, hal_can_queue_tx;
#if configSUPPORT_STATIC_ALLOCATION
StaticQueue_t hal_can_queue_rx_data;
uint8_t hal_can_queue_rx_buffer[HAL_CAN_QUEUE_RX_LEN*sizeof(can_message_t)];
StaticQueue_t hal_can_queue_tx_data;
uint8_t hal_can_queue_tx_buffer[HAL_CAN_QUEUE_TX_LEN*sizeof(can_message_t)];
#endif

uint32_t hal_can_tx_counter;
uint32_t hal_can_rx_counter;

uint32_t can_baudrate=HAL_CAN_BAUDRATE_DEF;

static uint8_t _inited;

CAN_HandleTypeDef hcan;

/* configuration table */
typedef struct {
    uint16_t kbps;
    uint16_t prescaler;
    uint32_t control;  /* SJW | BS1 | BS2 */
} can_baudrate_config_t;

#include "hal_can_baudrates.c.inc"


int hal_can_init( void )
{
    GPIO_InitTypeDef gpio_init;
    CAN_FilterTypeDef can_filter_init;
    
    if( _inited )
        return 1;

#if configSUPPORT_STATIC_ALLOCATION
    hal_can_queue_rx = xQueueCreateStatic( HAL_CAN_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(can_message_t),
                                           hal_can_queue_rx_buffer, &hal_can_queue_rx_data );
    hal_can_queue_tx = xQueueCreateStatic( HAL_CAN_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(can_message_t),
                                           hal_can_queue_tx_buffer, &hal_can_queue_tx_data );
#else
    hal_can_queue_rx = xQueueCreate( HAL_CAN_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(can_message_t) );
    hal_can_queue_tx = xQueueCreate( HAL_CAN_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(can_message_t) );
#endif
    if( !hal_can_queue_rx || !hal_can_queue_tx )
        return 0;
    vQueueAddToRegistry( hal_can_queue_rx, "canrxQ" );
    vQueueAddToRegistry( hal_can_queue_tx, "cantxQ" );

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

    // clock
    HAL_CAN_RCC_GPIO_ENABLE_CMD( HAL_CAN_RCC_GPIO_ENABLE_BIT );
    HAL_CAN_RCC_CAN_ENABLE_CMD( HAL_CAN_RCC_CAN_ENABLE_BIT );

    // alternative output port
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Pin = HAL_CAN_RX_PIN;
    gpio_init.Alternate = HAL_CAN_RX_AF;
    HAL_GPIO_Init( HAL_CAN_RX_PORT, &gpio_init );
    gpio_init.Pin = HAL_CAN_TX_PIN;
    gpio_init.Alternate = HAL_CAN_TX_AF;
    HAL_GPIO_Init( HAL_CAN_TX_PORT, &gpio_init );

    /* handle prepare */
    hcan.Instance = HAL_CANx;
  
    /* start with default baudrate */
    hal_can_set_baudrate(can_baudrate);

    /* filter init */
    can_filter_init.FilterBank = 0;
    can_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_init.FilterIdHigh = 0;
    can_filter_init.FilterIdLow = 0;
    can_filter_init.FilterMaskIdHigh = 0;
    can_filter_init.FilterMaskIdLow = 0;
    can_filter_init.FilterFIFOAssignment = CAN_FILTER_FIFO1;
    can_filter_init.FilterActivation = ENABLE;
    HAL_CAN_ConfigFilter( &hcan, &can_filter_init );

    /* Interrupt Enable */  
    __HAL_CAN_ENABLE_IT( &hcan, CAN_IT_RX_FIFO1_MSG_PENDING );  /* Receive messages pending */
    __HAL_CAN_ENABLE_IT( &hcan, CAN_IT_TX_MAILBOX_EMPTY );   /* Transmit mailbox empty */
    HAL_NVIC_SetPriority( HAL_CAN_RX_IRQn, 10, 0 );
    HAL_NVIC_EnableIRQ( HAL_CAN_RX_IRQn );
    HAL_NVIC_SetPriority( HAL_CAN_TX_IRQn, 10, 0 );
    HAL_NVIC_EnableIRQ( HAL_CAN_TX_IRQn );

    HAL_CAN_Start( &hcan );

    _inited = 1;
    return 1;
}

 
void hal_can_deinit( void )
{
    GPIO_InitTypeDef gpio_init;

    if( _inited )
    {
        vQueueDelete( hal_can_queue_rx );
        vQueueDelete( hal_can_queue_tx );
        hal_can_queue_rx = 0;
        hal_can_queue_tx = 0;

        HAL_CAN_DeInit( &hcan );

        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Mode = GPIO_MODE_INPUT;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Pin = HAL_CAN_RX_PIN;
        HAL_GPIO_Init( HAL_CAN_RX_PORT, &gpio_init );
        gpio_init.Pin = HAL_CAN_TX_PIN;
        HAL_GPIO_Init( HAL_CAN_TX_PORT, &gpio_init );

        _inited = 0;
    }
}


void hal_can_reset( void )
{
    HAL_CAN_Stop( &hcan );
    HAL_CAN_ResetError( &hcan );
    hal_can_tx_counter = 0;
    hal_can_rx_counter = 0;
    HAL_CAN_Start( &hcan );
}


int hal_can_set_baudrate( int baudrate )
{
    int i;

    for( i=0; i<sizeof(baudrate_config)/sizeof(can_baudrate_config_t); i++ )
    {
        if( baudrate_config[i].kbps*1000 == baudrate )
        {
            if( _inited )
                HAL_CAN_Stop( &hcan );
            else
                HAL_CAN_DeInit( &hcan );
            hcan.Init.Mode = CAN_MODE_NORMAL;
            hcan.Init.Prescaler = baudrate_config[i].prescaler;
            hcan.Init.SyncJumpWidth = baudrate_config[i].control;
            hcan.Init.TimeSeg1 = baudrate_config[i].control;
            hcan.Init.TimeSeg2 = baudrate_config[i].control;
            hcan.Init.TimeTriggeredMode = DISABLE;
            hcan.Init.AutoBusOff = DISABLE;
            hcan.Init.AutoWakeUp = DISABLE;
            hcan.Init.AutoRetransmission = DISABLE;
            hcan.Init.ReceiveFifoLocked = DISABLE;
            hcan.Init.TransmitFifoPriority = DISABLE;
            HAL_CAN_ResetError( &hcan );
            if( HAL_CAN_Init( &hcan ) != HAL_OK )
                return 0;
            can_baudrate = baudrate;
            if( _inited )
            {
                HAL_CAN_Start( &hcan );
            }
            return 1;
        }
    }
    return 0;
}


int hal_can_get_baudrate( void )
{
    return can_baudrate;
}


int hal_can_filter_set( int index, can_filter_t *filter, int enabled )
{
    CAN_FilterTypeDef can_filter_init;
    int f, m;

    if( (index < 0) || (index >= 14) )
        return 0;
    can_filter_init.FilterBank = index;
    can_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;
    if( filter->ext )
    {
        f = ((filter->ext_id & 0x1FFFFFFF)<<3) | 4;
        m = ((filter->ext_id_mask & 0x1FFFFFFF)<<3) |4;
    }
    else
    {
        f = (filter->std_id & 0x7FF)<<21;
        m = (filter->std_id_mask & 0x7FF)<<21;
    }
    if( filter->remote )
    {
        f |= 2;
        m |= 2;
    }        
    can_filter_init.FilterIdHigh = (f>>16) & 0xFFFF;
    can_filter_init.FilterIdLow = f & 0xFFFF;
    can_filter_init.FilterMaskIdHigh = (m>>16) & 0xFFFF;
    can_filter_init.FilterMaskIdLow = m & 0xFFFF;
    can_filter_init.FilterFIFOAssignment = CAN_FILTER_FIFO1;
    can_filter_init.FilterActivation = enabled ? ENABLE : DISABLE;
    HAL_CAN_ConfigFilter( &hcan, &can_filter_init );
    return 1;
}


int hal_can_filter_get( int index, can_filter_t *filter, int *enabled )
{
    int f, m;

    if( (index < 0) || (index >= 14) )
        return 0;

    if( CAN1->FA1R & (1<<index) )
    {
        *enabled = 1;
        m = CAN1->sFilterRegister[index].FR2;
        f = CAN1->sFilterRegister[index].FR1 & m; 
        filter->ext = (m & 4) ? 1 : 0;
        filter->remote = (m & 2) ? 1 : 0;
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
    return HAL_CAN_GetError( &hcan );
}


int hal_can_send( can_message_t *msg )
{
    CAN_TxHeaderTypeDef txmsg;
    uint8_t data[8];
    uint32_t i;

    if( (msg->len < 0) || (msg->len > 8) )
        return -1;

    /* transmit */
    if( msg->ext )
        txmsg.ExtId = msg->id;  /* ignored for CAN_Id_Standard */
    else
        txmsg.StdId = msg->id;  /* ignored for CAN_Id_Extended */
    txmsg.RTR = msg->remote ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    txmsg.IDE = msg->ext ? CAN_ID_EXT : CAN_ID_STD;
    txmsg.DLC = msg->len;
    for( i=0; i<msg->len; i++ )
        data[i] = msg->data[i];

    if( HAL_CAN_AddTxMessage( &hcan, &txmsg, data, &i ) == HAL_ERROR )
        return -1;
    else
        return i;
}


void hal_can_cancel( int id )
{
    HAL_CAN_AbortTxRequest( &hcan, id );
}


int hal_can_receive( can_message_t *msg )
{
    CAN_RxHeaderTypeDef rxmsg;
    uint8_t data[8];

    if( HAL_CAN_GetRxFifoFillLevel( &hcan, CAN_RX_FIFO1 ) == 0 )
        return 0;
    if( HAL_CAN_GetRxMessage( &hcan, CAN_RX_FIFO1, &rxmsg, data ) == HAL_ERROR )
        return 0;
    
    msg->ext = (rxmsg.IDE == CAN_ID_EXT) ? 1 : 0;
    if( msg->ext )
        msg->id = rxmsg.ExtId;
    else
        msg->id = rxmsg.StdId;
    msg->remote = (rxmsg.RTR == CAN_RTR_REMOTE) ? 1 : 0;
    msg->len = rxmsg.DLC;
    if( msg->len )
        memcpy( (void*)msg->data, data, msg->len );
    return 1;
}


int hal_can_read( can_message_t *msg, int block_time )
{
    if( xQueueReceive( hal_can_queue_rx, msg, block_time ) == pdPASS )
        return 1;
    else
        return 0;
}


int hal_can_write( can_message_t *msg )
{
    if( hal_can_send( msg ) >= 0 )
        return 1;
    /* mailbox busy, cache to queue */
    if( xQueueSend( hal_can_queue_tx, msg, 0 ) == pdPASS )
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
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    can_message_t msg;
    uint32_t tsrflags = READ_REG(hcan.Instance->TSR);

    while( 1 )
    {
        if( tsrflags & CAN_TSR_RQCP0 )
        {
            __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_RQCP0);
        }
        else if ( tsrflags & CAN_TSR_RQCP1 )
        {
            __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_RQCP0);
        }
        else if ( tsrflags & CAN_TSR_RQCP2 )
        {
            __HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_RQCP2);
        }
        else
            break;
 
        /* send remaining */ 
        if( xQueueReceiveFromISR( hal_can_queue_tx, &msg, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            if( hal_can_send( &msg ) < 0 )
                break;
            hal_can_tx_counter += 1;
        }
        else
        {
            hal_can_tx_counter += 1;
            break;
        }
    }
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void HAL_CAN_RX_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    can_message_t msg;

    while( hal_can_receive( &msg ) )
    {
        hal_can_rx_counter += 1;
        if( xQueueSendFromISR( hal_can_queue_rx, &msg, &xHigherPriorityTaskWoken ) != pdTRUE )
            break;
    }
    
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void CAN1_SCE1_IRQHandler(void)
{

}




#endif

