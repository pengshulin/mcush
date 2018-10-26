#include "mcush.h"
#if MCUSH_FATFS
#include "diskio.h"


DWORD get_fattime(void)
{
    uint32_t tick;
    if( get_rtc_tick(&tick) )
        return (DWORD)tick;
    else
        return 0;
}

// Get Drive Status
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	//DSTATUS stat;
	//int result;


	return STA_NOINIT;
}



// Inidialize a Drive
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{

	return STA_NOINIT;
}



// Read Sector(s)
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	return RES_PARERR;
}



// Write Sector(s)
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{

	return RES_PARERR;
}



// Miscellaneous Functions
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

	return RES_PARERR;
}

#endif

