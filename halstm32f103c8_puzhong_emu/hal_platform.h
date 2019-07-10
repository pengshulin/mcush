#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

//#define HAL_LED_NUM  1

//#if defined(BLACK_BOARD)
//    /* black board */
//    #define HAL_LED_PORTS  { 1 }
//    #define HAL_LED_PINS  { 12 }
//    #define HAL_LED_REV
//#elif defined(BLUE_BOARD)
//    /* blue board */
//    #define HAL_LED_PORTS  { 2 }
//    #define HAL_LED_PINS  { 13 }
//    #define HAL_LED_REV
//#else
//    #error "which board?"
//#endif

#define HAL_LED_NUM  2
#define HAL_LED_PORTS  { 1, 1 }
#define HAL_LED_PINS  { 12, 11 }


#ifndef USE_CMD_SGPIO
    #define USE_CMD_SGPIO   1
#endif


#endif
