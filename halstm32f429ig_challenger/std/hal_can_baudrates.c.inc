// PCLK1 = SYSCLK(180M) / 4 = 45M
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


