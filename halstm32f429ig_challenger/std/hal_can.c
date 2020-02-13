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
    #define HAL_CAN_RCC_GPIO_ENABLE_CMD    RCC_AHB1PeriphClockCmd
    #define HAL_CAN_RCC_GPIO_ENABLE_BIT    RCC_AHB1Periph_GPIOB
    #define HAL_CAN_RCC_CAN_ENABLE_CMD     RCC_APB1PeriphClockCmd
    #define HAL_CAN_RCC_CAN_ENABLE_BIT     RCC_APB1Periph_CAN1
    #define HAL_CANx                       CAN1
    #define HAL_CAN_RX_PORT                GPIOB
    #define HAL_CAN_RX_PIN                 GPIO_Pin_8
    #define HAL_CAN_RX_PIN_SOURCE          GPIO_PinSource8
    #define HAL_CAN_RX_AF                  GPIO_AF_CAN1
    #define HAL_CAN_TX_PORT                GPIOB
    #define HAL_CAN_TX_PIN                 GPIO_Pin_9
    #define HAL_CAN_TX_PIN_SOURCE          GPIO_PinSource9
    #define HAL_CAN_TX_AF                  GPIO_AF_CAN1
    #define HAL_CAN_RX_IRQn                CAN1_RX1_IRQn
    #define HAL_CAN_RX_IRQHandler          CAN1_RX1_IRQHandler
    #define HAL_CAN_TX_IRQn                CAN1_TX_IRQn
    #define HAL_CAN_TX_IRQHandler          CAN1_TX_IRQHandler
#endif

#ifndef HAL_CAN_BAUDRATE_DEF
#define HAL_CAN_BAUDRATE_DEF  1000000
#endif

extern const GPIO_TypeDef * const ports[];

QueueHandle_t hal_can_queue_rx, hal_can_queue_tx;

uint32_t can_baudrate=HAL_CAN_BAUDRATE_DEF;

int hal_can_init( void )
{
    GPIO_InitTypeDef gpio_init;
    CAN_InitTypeDef can_init;
    CAN_FilterInitTypeDef can_filter_init;
    NVIC_InitTypeDef nvic_init;

    hal_can_queue_rx = xQueueCreate( HAL_CAN_QUEUE_RX_LEN, ( unsigned portBASE_TYPE ) sizeof(can_message_t) );
    hal_can_queue_tx = xQueueCreate( HAL_CAN_QUEUE_TX_LEN, ( unsigned portBASE_TYPE ) sizeof(can_message_t) );
    if( !hal_can_queue_rx || !hal_can_queue_tx )
        return 0;
    vQueueAddToRegistry( hal_can_queue_rx, "canrxQ" );
    vQueueAddToRegistry( hal_can_queue_tx, "cantxQ" );

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    // clock
    HAL_CAN_RCC_GPIO_ENABLE_CMD( HAL_CAN_RCC_GPIO_ENABLE_BIT, ENABLE );
    HAL_CAN_RCC_CAN_ENABLE_CMD( HAL_CAN_RCC_CAN_ENABLE_BIT, ENABLE );

    // alternative output port
    gpio_init.GPIO_Speed = GPIO_High_Speed;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_Pin = HAL_CAN_RX_PIN;
    GPIO_Init( HAL_CAN_RX_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_CAN_TX_PIN;
    GPIO_Init( HAL_CAN_TX_PORT, &gpio_init );
    GPIO_PinAFConfig( HAL_CAN_RX_PORT, HAL_CAN_RX_PIN_SOURCE, HAL_CAN_RX_AF );
    GPIO_PinAFConfig( HAL_CAN_TX_PORT, HAL_CAN_TX_PIN_SOURCE, HAL_CAN_TX_AF );

    /* CAN register init */
    CAN_DeInit( HAL_CANx );
    CAN_StructInit( &can_init );
   
    /* CAN cell init */
    hal_can_set_baudrate(can_baudrate);
  
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

    return 1;
}

 
void hal_can_deinit( void )
{
    //hal_gpio_set_input( HAL_CAN_RX_PORT, 1<<HAL_CAN_RX_PIN );
    //hal_gpio_set_input( HAL_CAN_TX_PORT, 1<<HAL_CAN_TX_PIN );
}

/* configuration table */
typedef struct {
    uint32_t bps;
    uint32_t prescaler:10;  /* 1~1023 */
    uint32_t SWJ:2;  /* resynchronization jump width */
    uint32_t BS2:3;  /* time segment 2 */
    uint32_t BS1:4;  /* time segment 1 */
} can_baudrate_config_t;

// baudrate = PCLK1 / (SJW+BS1+BS2) / Prescaler
const can_baudrate_config_t baudrate_config[] = {
{ 1000000,    5, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  500000,   10, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  250000,   20, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  200000,   25, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  125000,   40, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  100000,   50, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   50000,  100, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   40000,  125, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   20000,  250, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   10000,  500, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{    5000, 1000, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
};


int hal_can_set_baudrate( int baudrate )
{
    CAN_InitTypeDef can_init;
    int i;

    for( i=0; i<sizeof(baudrate_config)/sizeof(can_baudrate_config_t); i++ )
    {
        if( baudrate_config[i].bps == baudrate )
        {
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
            can_baudrate = baudrate;
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
    CAN_FilterInitTypeDef can_filter_init;
    int f, m;

    if( (index < 0) || (index >= 14) )
        return 0;
    can_filter_init.CAN_FilterNumber = index;
    can_filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
    can_filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
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


int hal_can_get_last_error( void )
{
    return CAN_GetLastErrorCode(HAL_CANx);
}


int hal_can_get_error_count( void )
{
    return CAN_GetReceiveErrorCounter(HAL_CANx);
}


int hal_can_send( can_message_t *msg )
{
    CanTxMsg txmsg;
    int i;

    if( (msg->len < 0) || (msg->len > 8) )
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
    CAN_CancelTransmit( HAL_CANx, id );
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


void HAL_CAN_TX_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    can_message_t msg;

    if( CAN_GetITStatus( HAL_CANx, CAN_IT_TME) == SET )
    {
        CAN_ClearITPendingBit( HAL_CANx, CAN_IT_TME );
        /* send remaining */ 
        while( xQueueReceiveFromISR( hal_can_queue_tx, &msg, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            if( hal_can_send( &msg ) < 0 )
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
        if( xQueueSendFromISR( hal_can_queue_rx, &msg, &xHigherPriorityTaskWoken ) != pdTRUE )
            break;
    }
    
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void CAN1_SCE1_IRQHandler(void)
{

}




#endif

