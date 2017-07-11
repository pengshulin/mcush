#include "mcush.h"
#include "task_blink.h"
#include "spi_flash.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define  FLASH_WRITE_ADDRESS      0x700000
#define  FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define  FLASH_SECTOR_TO_ERASE    FLASH_WRITE_ADDRESS

#define  BufferSize (countof(Tx_Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx_Buffer[] = "STM32F4xx SPI Firmware Library Example: communication with an M25P SPI FLASH";
uint8_t  Rx_Buffer[BufferSize];
__IO uint8_t Index = 0x0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
__IO uint32_t FlashID = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return PASSED;
}


int cmd_spiffs( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "command", 'c', 0, "erase|read|write", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *cmd=0;
    void *addr=(void*)-1;
    char buf[256];
    int i, j;
    int len;
    void *p;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "command" ) == 0 )
                cmd = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd )
        return 1;

    if( strcmp( cmd, "erase" ) == 0 )
    {
        sFLASH_EraseBulk();
    }
    else if( strcmp( cmd, "read" ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        sFLASH_ReadBuffer( (void*)buf, (int)addr, 256 );
        for( i=0; i<16; i++ )
        {
            shell_printf( "%08X: ", (unsigned int)((int)addr+i*16) );
            for( j=0; j<16; j++ )
                shell_printf( "%02X ", *(unsigned char*)&buf[i*16+j] );
            shell_write_str( "\r\n" );
        }
    } 
    else if( strcmp( cmd, "write" ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        if( shell_make_16bits_data_buffer( &p, &len ) ) 
        {
            len *= 2;
            sFLASH_WritePage( (uint8_t*)p, (int)addr, len > 256 ? 256 : len );
            free( p );
        }
        else
            return 1;
    } 



    return 0;
}


const shell_cmd_t cmd_tab[] = {
{   0, 's', "spiffs",  cmd_spiffs, 
    "spiffs test",
    "spiffs"  },
{   CMD_END  } };


int main(void)
{
    mcush_init();
    task_blink_init();

    /* Initialize the SPI FLASH driver */
    sFLASH_Init();
    
    if( sFLASH_ReadID() != sFLASH_W25P64_ID )
        set_errno( 100 ); 

    ///* Get SPI Flash ID */
    //FlashID = sFLASH_ReadID();

    ///* Check the SPI Flash ID */
    //if( FlashID == sFLASH_W25P64_ID )
    //{
    //    /* OK: Turn on LD1 */
    //    hal_led_set(0);
    //    /* Perform a write in the Flash followed by a read of the written data */
    //    /* Erase SPI FLASH Sector to write on */
    //    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);

    //    /* Write Tx_Buffer data to SPI FLASH memory */
    //    sFLASH_WriteBuffer(Tx_Buffer, FLASH_WRITE_ADDRESS, BufferSize);

    //    /* Read data from SPI FLASH memory */
    //    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);

    //    /* Check the correctness of written dada */
    //    TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
    //    /* TransferStatus1 = PASSED, if the transmitted and received data by SPI1
    //       are the same */
    //    /* TransferStatus1 = FAILED, if the transmitted and received data by SPI1
    //       are different */

    //    /* Perform an erase in the Flash followed by a read of the written data */
    //    /* Erase SPI FLASH Sector to write on */
    //    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);

    //    /* Read data from SPI FLASH memory */
    //    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);

    //    /* Check the correctness of erasing operation dada */
    //    for (Index = 0; Index < BufferSize; Index++)
    //    {
    //        if (Rx_Buffer[Index] != 0xFF)
    //        {
    //            TransferStatus2 = FAILED;
    //        }
    //    }
    //    /* TransferStatus2 = PASSED, if the specified sector part is erased */
    //    /* TransferStatus2 = FAILED, if the specified sector part is not well erased */
    //}
    //else
    //{
    //    /* Error: Turn on LD2 */
    //    //STM_EVAL_LEDOn(LED2);
    //    hal_led_set( 1 );
    //}

    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);


}

