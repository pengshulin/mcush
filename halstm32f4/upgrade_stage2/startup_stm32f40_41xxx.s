/* Minimal startup script for upgrade stage2:
   After you loaded it into sram, 
   fetch the entry point (Reset_Handler) at 0x20000000,
   execute it and it will directly jump to main function.
   Peng Shulin <trees_peng@163.com> 2018
*/
  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  

  bl  main
  bx  lr    /* should never run here */
.size  Reset_Handler, .-Reset_Handler

   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors
    
    
g_pfnVectors:
  .word  Reset_Handler
  .word  0
