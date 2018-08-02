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
char *get_rtc_str(char *buf);
int set_rtc_by_str( char *s );
int set_rtc_by_val( int year, int mon, int mday, int hour, int min, int sec );
int get_rtc_tick( uint32_t *tick );
int get_rtc_tick64( uint64_t *tick );

char *rstrip( char *s );
char *lstrip( char *s );
char *strip( char *s );

int split_url( const char *url, char **protocol, char **server, int *port, char **pathfile );


#endif



