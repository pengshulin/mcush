#include "hal.h"


void hal_lcd_ports_init(void)
{
    GPIO_InitTypeDef init;
    
    //PB0 - backlight
    init.GPIO_Pin = GPIO_Pin_0;
    init.GPIO_Mode = GPIO_Mode_OUT;
    init.GPIO_OType = GPIO_OType_PP;;
    init.GPIO_Speed = GPIO_Speed_100MHz;
    init.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &init);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);  
 
    //PC5 - reset
    init.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &init);
    GPIO_SetBits(GPIOC, GPIO_Pin_5);  
 
    //PG12 - cs
    init.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &init);
    GPIO_SetBits(GPIOG, GPIO_Pin_12);  
     
    //PG0 - rs
    init.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOG, &init);
    GPIO_SetBits(GPIOG, GPIO_Pin_0);  
          
    //PD4 - rd
    init.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOD, &init);
    GPIO_SetBits(GPIOD, GPIO_Pin_4);  
     
    //PD5 - wr
    init.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &init);
    GPIO_SetBits(GPIOD, GPIO_Pin_5);  
     
    //PD10 PD9 PD8    PE15 PE14 PE13 PE12 PE11 PE10 PE9 PE8 PE7     PD1 PD0 PD15 PD14 - data
    init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_15 | GPIO_Pin_14;
    GPIO_Init(GPIOD, &init);
    init.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7;
    GPIO_Init(GPIOE, &init);
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


void hal_lcd_port_cs(int sta)
{
    if(!sta)
        GPIO_ResetBits(GPIOG, GPIO_Pin_12);
    else
        GPIO_SetBits(GPIOG, GPIO_Pin_12);
}


void hal_lcd_port_rs(int sta)
{
    if(!sta)
        GPIO_ResetBits(GPIOG, GPIO_Pin_0);
    else
        GPIO_SetBits(GPIOG, GPIO_Pin_0);
}


void hal_lcd_port_rd(int sta)
{
    if(!sta)
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    else
        GPIO_SetBits(GPIOD, GPIO_Pin_4);
}


void hal_lcd_port_wr(int sta)
{
    if(!sta)
        GPIO_ResetBits(GPIOD, GPIO_Pin_5);
    else
        GPIO_SetBits(GPIOD, GPIO_Pin_5);
}


void hal_lcd_port_senddata(uint16_t data)
{
    GPIO_SetBits(GPIOD, ((data)/0x2000)<<8 & 0x0700);
    GPIO_ResetBits(GPIOD, (~((data)/0x2000)<<8 & 0x0700));
    GPIO_SetBits(GPIOE, ((data<<3)/0x80)<<7 & 0xff80);
    GPIO_ResetBits(GPIOE, (~((data<<3)/0x80)<<7 & 0xff80));
    GPIO_SetBits(GPIOD, ((data<<12)/0x4000) & 0x0003);
    GPIO_ResetBits(GPIOD, (~((data<<12)/0x4000) & 0x0003));
    GPIO_SetBits(GPIOD, ((data<<14)/0x4000)<<14 & 0xC000);
    GPIO_ResetBits(GPIOD, (~((data<<14)/0x4000)<<14 & 0xC000));
}

#define  LCD_RESET_H  hal_lcd_port_reset(1)
#define  LCD_RESET_L  hal_lcd_port_reset(0)
#define  LCD_CS_H     hal_lcd_port_cs(1)
#define  LCD_CS_L     hal_lcd_port_cs(0)
#define  LCD_RS_H     hal_lcd_port_rs(1)
#define  LCD_RS_L     hal_lcd_port_rs(0)
#define  LCD_RD_H     hal_lcd_port_rd(1)
#define  LCD_RD_L     hal_lcd_port_rd(0)
#define  LCD_WR_H     hal_lcd_port_wr(1)
#define  LCD_WR_L     hal_lcd_port_wr(0)
#define  LCD_SEND(x)  hal_lcd_port_senddata((x))


void hal_lcd_write_reg(uint16_t dat)
{
    LCD_CS_L;
    LCD_RS_L;
    LCD_SEND(dat);
    LCD_WR_L;
    LCD_WR_H;
    LCD_CS_H;
}


void hal_lcd_write_data(uint16_t dat)
{
    LCD_CS_L;
    LCD_RS_H;
    LCD_SEND(dat);
    LCD_WR_L;
    LCD_WR_H;
    LCD_CS_H;
}



void hal_lcd_reset(void)
{
    LCD_RESET_L;
    hal_delay_ms(150);
    LCD_RESET_H;
    hal_delay_ms(50);
}


void hal_lcd_init(void)
{
    hal_lcd_ports_init();
    hal_lcd_backlight(0);
    hal_lcd_reset();

    hal_lcd_write_reg(0xCF);  
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0xC1); 
    hal_lcd_write_data(0X30); 
    hal_lcd_write_reg(0xED);  
    hal_lcd_write_data(0x64); 
    hal_lcd_write_data(0x03); 
    hal_lcd_write_data(0X12); 
    hal_lcd_write_data(0X81); 
    hal_lcd_write_reg(0xE8);  
    hal_lcd_write_data(0x85); 
    hal_lcd_write_data(0x10); 
    hal_lcd_write_data(0x7A); 
    hal_lcd_write_reg(0xCB);  
    hal_lcd_write_data(0x39); 
    hal_lcd_write_data(0x2C); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x34); 
    hal_lcd_write_data(0x02); 
    hal_lcd_write_reg(0xF7);  
    hal_lcd_write_data(0x20); 
    hal_lcd_write_reg(0xEA);  
    hal_lcd_write_data(0x00); 
    hal_lcd_write_data(0x00); 
    hal_lcd_write_reg(0xC0);    //Power control 
    hal_lcd_write_data(0x1B);   //VRH[5:0] 
    hal_lcd_write_reg(0xC1);    //Power control 
    hal_lcd_write_data(0x01);   //SAP[2:0];BT[3:0] 
    hal_lcd_write_reg(0xC5);    //VCM control 
    hal_lcd_write_data(0x30);   //3F
    hal_lcd_write_data(0x30);   //3C
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


void hal_lcd_openwin(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    hal_lcd_write_reg(0x2A); 
    hal_lcd_write_data(y0>>8);
    hal_lcd_write_data(0x00FF&y0);       
    hal_lcd_write_data(y1>>8);
    hal_lcd_write_data(0x00FF&y1);
    hal_lcd_write_reg(0x2B); 
    hal_lcd_write_data(x0>>8);
    hal_lcd_write_data(0x00FF&x0);       
    hal_lcd_write_data(x1>>8);
    hal_lcd_write_data(0x00FF&x1);
    hal_lcd_write_reg(0x2C);
}


void hal_lcd_clrscr(uint16_t backcolor)
{
    uint16_t i,j;
    hal_lcd_openwin(0, 0, LCD_ROW_NUM-1, LCD_COL_NUM-1);
    for(i = 0; i < LCD_ROW_NUM; i++)
        for(j =0; j < LCD_COL_NUM; j++)
            hal_lcd_write_data(backcolor);
}


void hal_lcd_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t i,j;
    hal_lcd_openwin(x0, y0, x1, y1);
    for(i = 0; i <= x1-x0; i++)
        for(j = 0; j <= y1-y0; j++)
            hal_lcd_write_data(color);
}


void hal_lcd_pixel(uint16_t x0, uint16_t y0, uint16_t color)
{
    hal_lcd_openwin(x0, y0, x0, y0);
    hal_lcd_write_data(color);
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


