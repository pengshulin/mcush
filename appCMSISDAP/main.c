//#include "cmsis_os.h"
//#include "osObjects.h"
//#include "rl_usb.h"
#include "DAP_config.h"
#include "DAP.h"
#include "mcush.h"

// Main program
int main (void) {

  DAP_Setup();                          // DAP Setup 

  USBD_Initialize(0U);                  // USB Device Initialization
  USBD_Connect(0U);                     // USB Device Connect

  while (!USBD_Configured(0U));         // Wait for USB Device to configure

  LED_CONNECTED_OUT(1U);                // Turn on  Debugger Connected LED
  LED_RUNNING_OUT(1U);                  // Turn on  Target Running LED
  Delayms(500U);                        // Wait for 500ms
  LED_RUNNING_OUT(0U);                  // Turn off Target Running LED
  LED_CONNECTED_OUT(0U);                // Turn off Debugger Connected LED

  // Create HID Thread
  HID0_ThreadId = osThreadCreate(osThread(HID0_Thread), NULL);

  osThreadSetPriority(osThreadGetId(), osPriorityIdle);
  for (;;);                             // Endless Loop
}
