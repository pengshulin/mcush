/* EU-TFT3.2 controller: ili9341 */
#include "hal.h"


#define  LCD_REG        *((uint16_t*)(0x6C000000 | 0x000007FE))
#define  LCD_DAT        *((uint16_t*)(0x6C000000 | 0x00000800))


void hal_lcd_ports_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    // PB0 - backlight
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);  
 
    // PC5 - reset
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_5);  
 
    // 使能FSMC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG,   ENABLE);  
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);
    // PORTD复用推挽输出  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    // PORTE复用推挽输出  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    // PORTG复用推挽输出  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOG, &GPIO_InitStructure); 
    
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);   
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);   
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12 
   
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12 
   
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource0, GPIO_AF_FSMC);//PG0,AF0 
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC); //PG12,AF12  
        
    // FSMC时序设置
    readWriteTiming.FSMC_AddressSetupTime = 0x01;   
    readWriteTiming.FSMC_AddressHoldTime = 0x00;     
    readWriteTiming.FSMC_DataSetupTime = 0x0f;  
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;    
    writeTiming.FSMC_AddressSetupTime = 0x02;
    writeTiming.FSMC_AddressHoldTime = 0x00;        
    writeTiming.FSMC_DataSetupTime = 0x05;  
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;     
    // LCD配置
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; 
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming; 
    // 初始化FSMC 使能BANK
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}


void hal_lcd_backlight(int on)
{
    if(on)
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
    else
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}


void hal_lcd_port_reset(int sta)
{
    if(!sta)
        GPIO_ResetBits(GPIOC, GPIO_Pin_5);
    else
        GPIO_SetBits(GPIOC, GPIO_Pin_5);
}


inline void hal_lcd_write_reg(uint16_t reg)
{
    LCD_REG = reg;
}


inline void hal_lcd_write_data(uint16_t dat)
{
    LCD_DAT = dat;
}


inline uint16_t hal_lcd_read_data(void)
{
    return LCD_DAT;
}



void hal_lcd_reset(void)
{
    hal_lcd_port_reset(0);
    hal_delay_ms(150);
    hal_lcd_port_reset(1);
    hal_delay_ms(50);
}


void hal_lcd_init(void)
{
    hal_lcd_ports_init();
    hal_lcd_backlight(0);
    hal_lcd_reset();

    hal_lcd_write_reg(0xCF);  // Power control B
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0xC1); 
    hal_lcd_write_data(0X30); 
    
    hal_lcd_write_reg(0xED);  // Power on sequence control
    hal_lcd_write_data(0x64); 
    hal_lcd_write_data(0x03); 
    hal_lcd_write_data(0X12); 
    hal_lcd_write_data(0X81); 
    
    hal_lcd_write_reg(0xE8);  // Driver timing control A
    hal_lcd_write_data(0x85); 
    hal_lcd_write_data(0x10); 
    hal_lcd_write_data(0x7A); 
    
    hal_lcd_write_reg(0xCB);  // Power control A
    hal_lcd_write_data(0x39); 
    hal_lcd_write_data(0x2C); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x34); 
    hal_lcd_write_data(0x02); 
    
    hal_lcd_write_reg(0xF7);  // Pump ratio control
    hal_lcd_write_data(0x20); 
    
    hal_lcd_write_reg(0xEA);  // Driver timing control B
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x00); 
    
    hal_lcd_write_reg(0xC0);  //Power control 1
    hal_lcd_write_data(0x1B); //VRH[5:0] 
    
    hal_lcd_write_reg(0xC1);  //Power control 2
    hal_lcd_write_data(0x01); //SAP[2:0];BT[3:0] 
    
    hal_lcd_write_reg(0xC5);  //VCM control 
    hal_lcd_write_data(0x30); //3F
    hal_lcd_write_data(0x30); //3C
    
    hal_lcd_write_reg(0xC7);    //VCM control2 
    hal_lcd_write_data(0XB7); 
    
    hal_lcd_write_reg(0x36);    // Memory Access Control 
    hal_lcd_write_data(0x48); 
    
    hal_lcd_write_reg(0x3A);   
    hal_lcd_write_data(0x55); 
    
    hal_lcd_write_reg(0xB1);   
    hal_lcd_write_data(0x00);   
    hal_lcd_write_data(0x1A); 
    
    hal_lcd_write_reg(0xB6);    // Display Function Control 
    hal_lcd_write_data(0x0A); 
    hal_lcd_write_data(0xA2); 
    
    hal_lcd_write_reg(0xF2);    // 3Gamma Function Disable 
    hal_lcd_write_data(0x00); 
    
    hal_lcd_write_reg(0x26);    //Gamma curve selected 
    hal_lcd_write_data(0x01); 
    
    hal_lcd_write_reg(0xE0);    //Set Gamma 
    hal_lcd_write_data(0x0F); 
    hal_lcd_write_data(0x2A); 
    hal_lcd_write_data(0x28); 
    hal_lcd_write_data(0x08); 
    hal_lcd_write_data(0x0E); 
    hal_lcd_write_data(0x08); 
    hal_lcd_write_data(0x54); 
    hal_lcd_write_data(0XA9); 
    hal_lcd_write_data(0x43); 
    hal_lcd_write_data(0x0A); 
    hal_lcd_write_data(0x0F); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x00);    
         
    hal_lcd_write_reg(0XE1);    //Set Gamma 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x15); 
    hal_lcd_write_data(0x17); 
    hal_lcd_write_data(0x07); 
    hal_lcd_write_data(0x11); 
    hal_lcd_write_data(0x06); 
    hal_lcd_write_data(0x2B); 
    hal_lcd_write_data(0x56); 
    hal_lcd_write_data(0x3C); 
    hal_lcd_write_data(0x05); 
    hal_lcd_write_data(0x10); 
    hal_lcd_write_data(0x0F); 
    hal_lcd_write_data(0x3F); 
    hal_lcd_write_data(0x3F); 
    hal_lcd_write_data(0x0F); 
    
    hal_lcd_write_reg(0x2B); 
    hal_lcd_write_data(0x00);
    hal_lcd_write_data(0x00);
    hal_lcd_write_data(0x01);
    hal_lcd_write_data(0x3f);
    
    hal_lcd_write_reg(0x2A); 
    hal_lcd_write_data(0x00);
    hal_lcd_write_data(0x00);
    hal_lcd_write_data(0x00);
    hal_lcd_write_data(0xef);   
      
    hal_lcd_write_reg(0x11); //Exit Sleep
    
    hal_delay_ms(120);
    
    hal_lcd_write_reg(0x29); //display on
    
    hal_lcd_write_reg(0x36);  
    hal_lcd_write_data(0xC9);      

}


void hal_lcd_openwin(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t write_mode)
{
    hal_lcd_write_reg(0x2A);  // Column address set
    hal_lcd_write_data(y0>>8);      // SCH
    hal_lcd_write_data(0x00FF&y0);  // SCL   
    hal_lcd_write_data(y1>>8);      // ECH
    hal_lcd_write_data(0x00FF&y1);  // ECL
    
    hal_lcd_write_reg(0x2B);  // Page address set
    hal_lcd_write_data(x0>>8);      // SPH
    hal_lcd_write_data(0x00FF&x0);  // SPL
    hal_lcd_write_data(x1>>8);      // EPH
    hal_lcd_write_data(0x00FF&x1);  // EPL
    
    if( write_mode )
        hal_lcd_write_reg(0x2C);  // Memory write
    else
    {
        hal_lcd_write_reg(0x2E);  // Memory read
        hal_lcd_write_data(0xFF);
    }
}


void hal_lcd_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t i,j;
    hal_lcd_openwin(x0, y0, x1, y1, 1);
    for(i = 0; i <= x1-x0; i++)
        for(j = 0; j <= y1-y0; j++)
            hal_lcd_write_data(color);
}


void hal_lcd_clrscr(uint16_t backcolor)
{
    hal_lcd_rectangle( 0, 0, LCD_ROW_NUM-1, LCD_COL_NUM-1, backcolor );
}


void hal_lcd_pixel(uint16_t x0, uint16_t y0, uint16_t color)
{
    hal_lcd_openwin(x0, y0, x0, y0, 1);
    hal_lcd_write_data(color);
}


uint16_t hal_lcd_get_pixel(uint16_t x0, uint16_t y0)
{
    hal_lcd_openwin(x0, y0, x0, y0, 0);
    return hal_lcd_read_data();
}


void hal_lcd_hline(uint8_t y, uint8_t x0, uint8_t x1, uint16_t color)
{
    hal_lcd_rectangle(x0, y, x1, y, color);
}


void hal_lcd_vline(uint8_t x, uint8_t y0, uint8_t y1, uint16_t color)
{
    hal_lcd_rectangle(x, y0, x, y1, color);
}


void hal_lcd_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int16_t a,b,next;
    a = 0;
    b = r;    
    next = 3 - (r<<1); 
    while(a <= b)
    {
        hal_lcd_pixel(x0+a, y0-b, color);
        hal_lcd_pixel(x0+b, y0-a, color);
        hal_lcd_pixel(x0+b, y0+a, color); 
        hal_lcd_pixel(x0+a, y0+b, color);
        hal_lcd_pixel(x0-a, y0+b, color);
        hal_lcd_pixel(x0-b, y0+a, color);
        hal_lcd_pixel(x0-a, y0-b, color); 
        hal_lcd_pixel(x0-b, y0-a, color);
        a++;
        //use the bresenham    
        if(next<0)
            next += 4*a+6;    
        else
        {
            next += 10+4*(a-b);   
            b--;
        }                           
    }
} 



/********************************************************************************************************
*  Function: Driver_TouchCS                                                                            
*  Object: touch ic spi enable/unable
*  Input: 1/0
*  Output: none                                                                                              
*  brief:   none                                 
********************************************************************************************************/
void Driver_SPICS(int sta)
{
        static int StartFlag = 1;
        if(StartFlag)
        {
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            StartFlag = 0;
            //FCS/SDCS UNABLE
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
            GPIO_SetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3);
        }   
        if(!sta)
            GPIO_ResetBits(GPIOB, GPIO_Pin_2);
        else
            GPIO_SetBits(GPIOB, GPIO_Pin_2);
}

/********************************************************************************************************
*  Function: Driver_SPIMISO                                                                            
*  Object: SPI_MOSI
*  Input: none
*  Output: 1/0                                                                                           
*  brief:   none                                   
********************************************************************************************************/
int Driver_SPIMISO(void)
{
        static int StartFlag = 1;
        if(StartFlag)
        {   
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
            GPIO_Init(GPIOF, &GPIO_InitStructure);
            StartFlag = 0;
        }
        return GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_8);
}

/********************************************************************************************************
*  Function: Driver_SPIMOSI                                                                            
*  Object: SPI_MOSI
*  Input: 1/0
*  Output: none                                                                                              
*  brief:   none                                   
********************************************************************************************************/
void Driver_SPIMOSI(int sta)
{
        static int StartFlag = 1;
        if(StartFlag)
        {
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
            GPIO_Init(GPIOF, &GPIO_InitStructure);
            StartFlag = 0;
        }   
        if(!sta)
            GPIO_ResetBits(GPIOF, GPIO_Pin_9);
        else
            GPIO_SetBits(GPIOF, GPIO_Pin_9);
}

/********************************************************************************************************
*  Function: Driver_SPISCK                                                                             
*  Object: SPI_SCK
*  Input: 1/0
*  Output: none                                                                                              
*  brief:   none                                      
********************************************************************************************************/
void Driver_SPISCK(int sta)
{
        static int StartFlag = 1;
        if(StartFlag)
        {
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;; 
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            StartFlag = 0;
        }   
        if(!sta)
            GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        else
            GPIO_SetBits(GPIOB, GPIO_Pin_1);
}




//触摸检测值 数据类型
typedef struct{
        uint16_t x;
        uint16_t y;  
}TOUCH_VAL_TYPE;
TOUCH_VAL_TYPE TouchVal;

//往SPI接口发送数据
void Touch_SPIWrite(uint8_t Val)
{
    uint8_t i;
    Driver_SPISCK(0);
    for(i=0; i<8; i++)
        {
                if(Val&0x80)
                    Driver_SPIMOSI(1);
                else
                    Driver_SPIMOSI(0);
                Val <<= 1;
        Driver_SPISCK(0);
        __NOP();
                __NOP();
                __NOP();
        Driver_SPISCK(1);
        __NOP();
                __NOP();
                __NOP();
    }
}

//从SPI接口接收数据
uint16_t Touch_SPIRead(void)
{
    uint8_t i;
    uint16_t Val = 0;
    for(i=0; i<12; i++)
    {
        Val <<= 1;
        Driver_SPISCK(1);
        __NOP();
        __NOP();
        __NOP();
        Driver_SPISCK(0);
        __NOP();
        __NOP();
        __NOP();
        if(Driver_SPIMISO())
             Val++;
    }
    return Val;
}

//读一次触摸检测值
void Touch_GetVal(TOUCH_VAL_TYPE *Point)
{
    Driver_SPICS(0);
    Touch_SPIWrite(0x90);   //读取Y轴 检测值
    Driver_SPISCK(1);
    __NOP();
    __NOP();
    __NOP();
    Driver_SPISCK(0);
    __NOP();
    __NOP();
    __NOP();
    (*Point).y = Touch_SPIRead();
    Touch_SPIWrite(0xd0);  //读取X轴 检测值
    Driver_SPISCK(1);
    __NOP();
    __NOP();
    __NOP();
    Driver_SPISCK(0);
    __NOP();
    __NOP();
    __NOP();
    (*Point).x = Touch_SPIRead();
    Driver_SPICS(1);
    __NOP();
    __NOP();
    __NOP();
}

//触摸数据采样处理 并转换为坐标
int Touch_GetSite(uint16_t *pRow, uint16_t *pColumn)
{
        TOUCH_VAL_TYPE Point[1];
        //读触摸采样值
        Touch_GetVal(&Point[0]);
        //将触摸ADC值 转换为屏对应像素坐标
        if((Point[0].x > 380 && Point[0].x < 3750) && (Point[0].y > 330 && Point[0].y < 3900))
        {
                //转换为像素坐标
                Point[0].x = (Point[0].x-380)/((3750-380)/240);         //3750->380
                Point[0].y = (Point[0].y-330)/((3900-330)/320);         //330->3900
                //转换为行列值
                *pRow = Point[0].y;
                *pColumn = 240-Point[0].x;
                return 1;
        }
    return 0;
}



void hal_touch_init(void)
{


}





