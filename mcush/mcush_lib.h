/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_LIB_H__
#define __MCUSH_LIB_H__


#ifdef __cplusplus
extern "C" {
#endif

extern void _halt_with_message(const char *message);
extern void _halt(void);
#if DEBUG
    #define halt(message)  _halt_with_message(message)
#else
    #define halt(message)  _halt() 
#endif

extern int get_errno(void);
extern int set_errno(int num);


uint32_t calc_checksum(void *p, uint32_t len);

//#define USE_REVERSE_TAB
uint32_t reverse_32_bits(uint32_t v);
uint16_t reverse_16_bits(uint16_t v);
uint8_t reverse_8_bits(uint8_t v);

int test_bit1_num(int v);
int test_bit1_position(int v, int msb);

void test_delay_us(void);
void test_delay_ms(void);

char *get_float_tick_time_str(char *buf, uint32_t tick);
char *get_tick_time_str(char *buf, uint32_t tick, int ms);
char *get_uptime_str(char *buf, int ms);
char *get_rtc_str(char *buf);
char *get_rtc_tick_str(char *buf, uint32_t tick);

int parse_long_int( const char *str, long int *i );
int parse_ptr( const char *str, uintptr_t *p );
int parse_int( const char *str, int *i );
int parse_int_repeat( const char *str, int *i, int *repeat );
int parse_float( const char *str, float *f );
int parse_date( const char *str, int *year, int *mon, int *mday );
int parse_time( const char *str, int *hour, int *min, int *sec );
int parse_date_time( const char *str, int *year, int *mon, int *mday, int *hour, int *min, int *sec );

int set_rtc_by_str( char *s );
int set_rtc_by_val( int year, int mon, int mday, int hour, int min, int sec );
int get_rtc_tick( uint32_t *tick );
int get_rtc_tick64( uint64_t *tick );

char *strdup2( const char *s );

char *rstrip( char *s );
char *lstrip( char *s );
char *strip( char *s );
char *split_line( char *head );
char *strip_line( char **head );

char *hexlify( const char *buf_in, char *buf_out, int len, int add_null_end );
int unhexlify( const char *buf_in, char *buf_out, int len );


int split_url( const char *url, char **protocol, char **server, int *port, char **pathfile );

void byte_to_unicode( uint8_t *buf_in, uint16_t *buf_out, int len, int add_null_end );

void swap_bytes( uint8_t *a, uint8_t *b );

int cmp_int8(const void *a, const void *b);
int cmp_int16(const void *a, const void *b);
int cmp_int32(const void *a, const void *b);
int cmp_uint8(const void *a, const void *b);
int cmp_uint16(const void *a, const void *b);
int cmp_uint32(const void *a, const void *b);
int cmp_float(const void *a, const void *b);

int bytes_to_int( char *bytes, int len );

typedef struct {
    float q;
    float r;
    float gain;
    float p_k_k1;
    float p_k1_k1;
    float x_k_k1;
    float x_k1_k1;
    float z_k;
    float oldval;
} kalman_filter_t;

void kalman_filter_init( kalman_filter_t *filter );
float kalman_filter_update( kalman_filter_t *filter, float newval );

#ifdef __cplusplus
}
#endif

#endif

