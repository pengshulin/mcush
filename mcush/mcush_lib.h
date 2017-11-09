/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _MCUSH_LIB_H_
#define _MCUSH_LIB_H_

uint32_t reverse_32_bits(uint32_t v);
uint16_t reverse_16_bits(uint16_t v);
uint8_t reverse_8_bits(uint8_t v);
uint32_t calc_checksum(void *p, uint32_t len);

void test_delay_us(void);
void test_delay_ms(void);

char *get_uptime_str(char *buf, int ms);

#endif



