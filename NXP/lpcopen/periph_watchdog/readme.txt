WWDT example

Example description
This example shows how to use Windowed watchdog timer to generate interrupts
to manage the watchdog timer. If the watchdog timer isn't 'fed' within it's
window time, it will reset the device. The LED will toggle states on each
watchdog feed event. You can also use a terminal program with the UART to
select the feed event. If the feed is disabled, the chip will reset.

No special setup is required for the demo. Just build and run.
NGX Xplorer users (no UART) should modify the code to test the different
watchdog events.

The example works in one of 4 ways. If '1' is selected, WDT is fed by the
sysTick at 20Hz and should never time out (fast LED). If '2' is selcted, the
WDT is fed by the watchdog window interrupt (slow LED). If '3' is selected, the
watchdog times out and resets the device. If '4' is selected, the interrupt
for the WDT timeout events are routed via the event router handler instead of
the watchdog interrupt handler.

Special connection requirements
There are no special connection requirements for this example.







