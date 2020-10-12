#ifndef __FLASH_CONFIG_H__
#define __FLASH_CONFIG_H__

/* Flash layout for STM32F103x8 64KB
 * 0x08000000 Sector 0  ---  4k  APP 
 * 0x08001000 Sector 1  ---  4k  APP
 * 0x08002000 Sector 2  ---  4k  APP
 * 0x08003000 Sector 3  ---  4k  APP
 * 0x08004000 Sector 4  ---  4k  APP
 * 0x08005000 Sector 5  ---  4k  APP
 * 0x08006000 Sector 6  ---  4k  APP 
 * 0x08007000 Sector 7  ---  4k  APP 
 * 0x08008000 Sector 8  ---  4k  APP 
 * 0x08009000 Sector 9  ---  4k  APP 
 * 0x0800A000 Sector 10 ---  4k  APP 
 * 0x0800B000 Sector 11 ---  4k  APP 
 * 0x0800C000 Sector 12 ---  4k  APP
 * 0x0800D000 Sector 13 ---  4k  APP
 * 0x0800E000 Sector 14 ---  4k  APP
 * 0x0800F000 Sector 15 ---  4k  
 * ......F000   Page 60 ---  1k  APP
 * ......F400   Page 61 ---  1k  APP
 * ......F800   Page 62 ---  1k  APP
 * ......FC00   Page 63 ---  1k  FCFS (Flash Configuration File System)
 */

#define FLASH_APP_ADDR_BASE             0x08000000  // Application
#define FLASH_APP_SIZE                  (63*1024)

#define FLASH_FCFS_ADDR_BASE            0x0800FC00  // FCFS
#define FLASH_FCFS_SIZE                 (1*1024)


#endif

