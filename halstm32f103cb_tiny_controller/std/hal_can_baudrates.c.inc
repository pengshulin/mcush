// pclk1 = sysclk(72M) / 4 = 36M
// baudrate = PCLK1(36M) / (SJW+BS1+BS2) / Prescaler
const can_baudrate_config_t baudrate_config[] = {
{ 1000000,   4, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  900000,   4, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_6tq },
{  800000,   5, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  600000,   5, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_8tq },
{  500000,   8, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  250000,  16, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  200000,  20, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  125000,  32, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{  100000,  40, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   50000,  80, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   40000, 100, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   20000, 200, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{   10000, 400, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
{    5000, 800, CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq },
};

