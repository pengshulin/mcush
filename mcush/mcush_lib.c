/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <math.h>



uint32_t calc_checksum(void *p, uint32_t len)
{
    uint32_t sum=0;
    uint8_t *b=(uint8_t*)p;

    while( len-- )
        sum += *b++;
    return sum;
}


#ifdef USE_REVERSE_TAB
static uint8_t _bits_reverse_tab[256] = {
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};
#endif



uint32_t reverse_32_bits(uint32_t v)
{
#ifdef USE_REVERSE_TAB
    uint32_t v2;
    uint8_t *p1=(uint8_t*)&v, *p2=(uint8_t*)&v2+3;
    *p2-- = _bits_reverse_tab[*p1++];
    *p2-- = _bits_reverse_tab[*p1++];
    *p2-- = _bits_reverse_tab[*p1++];
    *p2   = _bits_reverse_tab[*p1];
    return v2;
#else
    v = ((v & 0xFFFF0000)>>16) | ((v & 0x0000FFFF)<<16);  // ABCDEFGH -> EFGHABCD
    v = ((v & 0xFF00FF00)>>8)  | ((v & 0x00FF00FF)<<8);   // EFGHABCD -> GHEFCDAB
    v = ((v & 0xF0F0F0F0)>>4)  | ((v & 0x0F0F0F0F)<<4);   // GHEFCDAB -> HGFEDCBA
    v = ((v & 0xCCCCCCCC)>>2)  | ((v & 0x33333333)<<2);   // X: abcd -> X: cdab
    v = ((v & 0xAAAAAAAA)>>1)  | ((v & 0x55555555)<<1);   // X: cdab -> X: dcba
    return v;
#endif
}


uint16_t reverse_16_bits(uint16_t v)
{
#ifdef USE_REVERSE_TAB
    uint16_t v2;
    uint8_t *p1=(uint8_t*)&v, *p2=(uint8_t*)&v2+1;
    *p2-- = _bits_reverse_tab[*p1++];
    *p2   = _bits_reverse_tab[*p1];
    return v2;
#else
    v = (uint16_t)((v & 0xFF00)>>8) | (uint16_t)((v & 0x00FF)<<8);  // ABCD -> CDAB
    v = (uint16_t)((v & 0xF0F0)>>4) | (uint16_t)((v & 0x0F0F)<<4);  // CDAB -> DCBA
    v = (uint16_t)((v & 0xCCCC)>>2) | (uint16_t)((v & 0x3333)<<2);  // X: abcd -> X: cdab
    v = (uint16_t)((v & 0xAAAA)>>1) | (uint16_t)((v & 0x5555)<<1);  // X: cdab -> X: dcba
    return v;
#endif
}


uint8_t reverse_8_bits(uint8_t v)
{
#ifdef USE_REVERSE_TAB
    return _bits_reverse_tab[v];
#else
    v = (uint8_t)((v & 0xF0)>>4) | (uint8_t)((v & 0x0F)<<4);  // AB -> BA
    v = (uint8_t)((v & 0xCC)>>2) | (uint8_t)((v & 0x33)<<2);  // X: abcd -> X: cdab
    v = (uint8_t)((v & 0xAA)>>1) | (uint8_t)((v & 0x55)<<1);  // X: cdab -> X: dcba
    return v;
#endif
}


int test_bit1_num(int v)
{
    int i, j, r=0;
    for( i=0, j=0x1; i<32; i++, j<<=1 )
    {
        if( v & j )
            r++;
    }
    return r;
}


int test_bit1_position(int v, int msb)
{
    int i, j;
    if( msb )
    {
        for( i=32, j=0x80000000; i; i--, j>>=1 )
        {
            if( v & j )
                return i-1;
        }
    }
    else
    {
        for( i=0, j=0x1; i<32; i++, j<<=1 )
        {
            if( v & j )
                return i;
        }
    }
    return -1;
}


void test_delay_us(void)
{
    while(1)
    {
        hal_led_set(0);
        hal_delay_us(1);
        hal_led_clr(0);
        hal_delay_us(10);
        hal_led_set(0);
        hal_delay_us(100);
        hal_led_clr(0);
        hal_delay_us(1000);
    }
}


void test_delay_ms(void)
{
    while(1)
    {
        hal_led_toggle(0);
        hal_delay_ms(1000);
    }
}


char *get_float_tick_time_str(char *buf, uint32_t tick)
{
    float fs = tick;
    fs = fs / OS_TICK_RATE;
    sprintf(buf, "%.3f", fs );
    return buf;
}


char *get_tick_time_str(char *buf, uint32_t tick, int ms)
{
    unsigned int s = tick / OS_TICK_RATE;
    if( ms )
    {
        tick = tick - s * OS_TICK_RATE;
        tick = tick * 1000 / OS_TICK_RATE; 
        sprintf(buf, "%u:%02u:%02u.%03u", s/3600, (s/60)%60, s%60, (unsigned int)tick);
    }
    else
        sprintf(buf, "%u:%02u:%02u", s/3600, (s/60)%60, s%60);
    return buf;
}


char *get_uptime_str(char *buf, int ms)
{
    return get_tick_time_str( buf, os_tick(), ms );
}


char *get_rtc_str(char *buf)
{
#if HAL_RTC
    struct tm t;
    if( hal_rtc_get( &t ) )
    {
        sprintf( buf, "%d-%d-%d %02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday,
                        t.tm_hour, t.tm_min, t.tm_sec );
        return buf;
    }
#else
    (void)buf;
#endif
    return 0;
}


char *get_rtc_tick_str(char *buf, uint32_t tick)
{
#if HAL_RTC
    struct tm *t;
    uint32_t rt, s;

    if( get_rtc_tick( &rt ) )
    {
        s = (os_tick() - tick) / OS_TICK_RATE;
        rt = rt + 8*60*60 - s;
        t = gmtime( (const time_t*)&rt );
        t->tm_mon += 1;
        t->tm_year += 1900;
        sprintf( buf, "%d-%d-%d %02d:%02d:%02d", 
                 t->tm_year, t->tm_mon, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec );
        return buf;
    }
#else
    (void)buf;
    (void)tick;
#endif
    return 0;
}


/* parse signed integer in dec/hex mode */
int parse_int( const char *str, int *i )
{
    long long int r;
    char *p;
    if( !str || (*str==0) )
        return 0;
    r = strtoll( str, &p, 0 );
    if( (p==0) || (str==(const char*)p) )
        return 0;
    while( (*p==' ') || (*p=='\t') )
        p++;
    if( *p ) 
        return 0;
    //if( (r == LONG_MAX) || (r == LONG_MIN) )
    //    return 0;
    *i = (int)r;
    return 1;
}


/* parse signed integer (may with repeat number) in dec/hex mode */
int parse_int_repeat( const char *str, int *i, int *repeat )
{
    long long int r, r2;
    char *p;
    if( !str || (*str==0) )
        return 0;
    r = strtoll( str, &p, 0 );
    if( (p==0) || (str==(const char*)p) )
        return 0;
    while( (*p==' ') || (*p=='\t') )
        p++;
    if( *p=='*' )
    {
        p++;
        str = (const char*)p;
        r2 = strtoll( str, &p, 0 );
        if( (p==0) || (str==(const char*)p) )
            return 0;
        while( (*p==' ') || (*p=='\t') )
            p++;
        if( *p )
            return 0;
        if( r2 >= 1 )
            r2 -= 1;
    }
    else if( *p )
    {
        return 0;  /* missing repeat num */
    }
    else
        r2 = 0;  /* no repeat */
    //if( (r == LONG_MAX) || (r == LONG_MIN) )
    //    return 0;
    *i = (int)r;
    *repeat = (int)r2;
    return 1;
}


/* parse float */
int parse_float( const char *str, float *f )
{
    float r;
    char *p;
    if( !str )
        return 0;
    r = strtof( str, &p );
    if( !p )
        return 0;
    while( (*p==' ') || (*p=='\t') )
        p++;
    if( *p ) 
        return 0;
    *f = r;
    return 1;
}


/* format: "%d-%d-%d" */
int parse_date( const char *str, int *year, int *mon, int *mday )
{
    char *p=(char*)str, *p2;
    int y, m, d;

    y = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) || (*p2 != '-') )
        return 0;
    p = p2+1;
    m = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) || (*p2 != '-') )
        return 0;
    p = p2+1;
    d = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) )
        return 0;
    if( y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31 )
        return 0;
    *year = y;
    *mon = m;
    *mday = d;
    return 1;
}


/* format: "%d:%d:%d" */
int parse_time( const char *str, int *hour, int *min, int *sec )
{
    char *p=(char*)str, *p2;
    int h, m, s;

    h = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) || (*p2 != ':') )
        return 0;
    p = p2+1;
    m = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) || (*p2 != ':') )
        return 0;
    p = p2+1;
    s = strtol( (const char *)p, &p2, 10 );
    if( (p == p2) )
        return 0;
    if( h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59 )
        return 0;
    *hour = h;
    *min = m;
    *sec = s;
    return 1;
}


/* format: "%d-%d-%d %d:%d:%d" */
int parse_date_time( const char *str, int *year, int *mon, int *mday, int *hour, int *min, int *sec )
{
    char *p, *p2;

    /* split into two parts */ 
    p = p2 = lstrip((char*)str);
    if( *p == 0 )
        return 0;
    while( *p2 && (*p2 != ' ') )
        p2++;
    if( *p2 != ' ' )
        return 0;
    p2 = lstrip(p2);
    if( *p2 == 0 )
        return 0;
    return (parse_date( (const char*)p, year, mon, mday ) && parse_time( (const char*)p2, hour, min, sec)) ? 1 : 0;
}


int set_rtc_by_str( char *s )
{
#if HAL_RTC
    struct tm t;
   
    if( parse_date_time( s, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec ) )
    {
        /* format 18-1-1 --> 2018-1-1 */
        if( t.tm_year < 100 )
            t.tm_year += 2000;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = 0;
        if( hal_rtc_set( &t ) )
            return 1;
    }
#else
    (void)s;
#endif
    return 0;
}


int set_rtc_by_val( int year, int mon, int mday, int hour, int min, int sec )
{
#if HAL_RTC
    struct tm t;
    /* format 18-1-1 --> 2018-1-1 */
    if( year < 100 )
        t.tm_year = year + 2000;
    else
        t.tm_year = year;
    t.tm_mon = mon;
    t.tm_mday = mday;
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = sec;
    t.tm_wday = 0;
    t.tm_yday = 0;
    t.tm_isdst = 0;
    if( hal_rtc_set( &t ) )
        return 1;
#else
    (void)year;
    (void)mon;
    (void)mday;
    (void)hour;
    (void)min;
    (void)sec;
#endif
    return 0;
}


int get_rtc_tick( uint32_t *tick )
{
#if HAL_RTC
    struct tm t;
    if( ! hal_rtc_get( &t ) )
        return 0;
    t.tm_mon -= 1;
    t.tm_year -= 1900;
    *tick = mktime( &t );
    *tick -= 8*60*60;
    return 1; 
#else
    (void)tick;
    return 0; 
#endif
}


int get_rtc_tick64( uint64_t *tick )
{
#if HAL_RTC
    struct tm t;
    if( ! hal_rtc_get( &t ) )
        return 0;
    t.tm_mon -= 1;
    t.tm_year -= 1900;
    *tick = mktime( &t );
    *tick -= 8*60*60;
    return 1; 
#else
    (void)tick;
    return 0;
#endif
}


/* a safe version of strdup that calls malloc */
char *strdup2( const char *s )
{
    char *p = NULL;

    if( s != NULL )
    {
         p = (char *)os_malloc( strlen(s) + 1 );
         strcpy( p, s );
    }
    return p;
}


char *rstrip( char *s )
{
    unsigned int l;

    if( s == 0 )
        return 0;
    l = strlen(s);
    while( l )
    {
        switch( s[l-1] )
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            s[l-1] = 0;
            l -= 1;
            break;
        case 0:
        default:
            return s;
        }
    }
    return s;
}


char *lstrip( char *s )
{
    if( s == 0 )
        return 0;
    while( 1 )
    {
        switch( *s )
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            s++;
            break;
        case 0:
        default:
            return s;
        }
    }
}


char *strip( char *s )
{
    return rstrip(lstrip(s));
}


/* Split multi-line string into two parts: head line and the remaining tail lines.
   Terminate the head line and return the entire following string,
   seperated by '\r', '\n' or any combination
   NOTE:
   1) both two strings are not stripped for blank chars (SPACE, TAB)
   2) head line will be right stripped for NEWLINE char (\r, \n), 
      so it may be zero-length if it's really empty
   3) tail lines pointer will be left stripped for NEWLINE char (\r, \n)
 */
char *split_line( char *head )
{
    char *p; 
    int newline=0;

    if( (head == 0) || (*head==0) )
        return 0;
    p = head;
    while( *p )
    {
        if( (*p=='\n') || (*p=='\r') )
        {
            *p = 0;
            newline = 1;
        }
        else if(newline)
        {
            break;
        }
        p++;
    }
    return (newline && *p) ? p : 0;
}


/* similar to the split_line function, continue to strip both two parts,
   so argument should be the pointer to the head line pointer
 */
char *strip_line( char **head )
{
    char *tail;
    
    if( (head==0) || (*head==0) )
        return 0;
    tail = split_line( *head );
    if( **head != 0 )
        *head = strip(*head);
    if( tail && (*tail != 0) )
        tail = strip(tail);
    return tail;
}


char *hexlify( const char *buf_in, char *buf_out, int len, int add_null_end )
{
    char *p=buf_out, c;
    while( len-- )
    {
        c = (*buf_in >> 4) & 0x0F;
        *p++ = (c < 0x0A) ? ('0'+c) : ('A'+c-0x0A);
        c = *buf_in++ & 0x0F;
        *p++ = (c < 0x0A) ? ('0'+c) : ('A'+c-0x0A);
    }
    if( add_null_end )
        *p = 0;
    return buf_out;
}


int8_t c2i(char c)
{
    if( (c >= '0') && (c <= '9') )
        return c - '0';
    else if( (c >= 'A') && (c <= 'F') ) 
        return c - 'A' + 10;
    else if( (c >= 'a') && (c <= 'f') ) 
        return c - 'a' + 10;
    return -1;
}


int unhexlify( const char *buf_in, char *buf_out, int len )
{
    int8_t c, c2;
    int bytes=0;

    while( len-- )
    {
        c = c2i(*buf_in++);
        if( c == -1 )
            break;
        c2 = c2i(*buf_in++);
        if( c2 == -1 )
            break;
        *buf_out++ = (char)(c<<4) + c2;
        bytes++;
    }
    return bytes;
}


/* split url in form of <protocol>://<server>[:<port>]/<path...file>,
   check out protocol/server/path_file sub-strings and port
   return: 0 - success, others for error type
   NOTE: original url string is split into pieces, backup before function call (if necessary)
 */
int split_url( const char *url, char **protocol, char **server, int *port, char **pathfile )
{
    char *p, *p2=0;
    if( url == NULL || protocol == NULL || server == NULL || port == NULL || pathfile == NULL )
        return -1;
    *protocol = *server = *pathfile = 0;
    p = (char*)url;
    /* check out protocol */
    while( *p )
    {
        if( strncmp((const char*)p, "://", 3 ) == 0 )
        {
            *p = 0;
            *protocol = (char*)url;
            if( *(p+3) )
                *server = p+3;
            break;
        }
        else
            p++;
    }
    if( *protocol == 0 )
    {
        /* protocol not found, go back */
        *server = (char*)url;
    }
    /* check for server */
    if( **server == 0 )
        return 1;
    p = *server;
    while( *p )
    {
        if( *p == '/' )
        {
            *p = 0;
            if( *(p+1) )
                *pathfile = p+1;
            break;
        }
        else if( *p == ':' )
        {
            *p++ = 0;
            p2 = p;
        }
        else
            p++;
    }
    if( p2 && *p2 )
    {
        if( parse_int( (const char*)p2, port ) == 0 )
            return 2;
    }
    else
        *port = -1;
    //if( *pathfile == NULL )
    //    return 2;
    return 0;
}


void byte_to_unicode( uint8_t *buf_in, uint16_t *buf_out, int len, int add_null_end )
{
    while( len-- )
    {
        *buf_out++ = *buf_in++;
    }
    if( add_null_end )
        *buf_out= 0;
}


void swap_bytes( uint8_t *a, uint8_t *b )
{
    uint8_t c = *b;
    *b = *a;
    *a = c;
}


int cmp_int8(const void *a, const void *b)
{
    return (*(int8_t*)a > *(int8_t*)b) ? 1 : (*(int8_t*)a==*(int8_t*)b ? 0 : -1);
}


int cmp_int16(const void *a, const void *b)
{
    return (*(int16_t*)a > *(int16_t*)b) ? 1 : (*(int16_t*)a==*(int16_t*)b ? 0 : -1);
}


int cmp_int32(const void *a, const void *b)
{
    return (*(int32_t*)a > *(int32_t*)b) ? 1 : (*(int32_t*)a==*(int32_t*)b ? 0 : -1);
}


int cmp_uint8(const void *a, const void *b)
{
    return (*(uint8_t*)a > *(uint8_t*)b) ? 1 : (*(uint8_t*)a==*(uint8_t*)b ? 0 : -1);
}


int cmp_uint16(const void *a, const void *b)
{
    return (*(uint16_t*)a > *(uint16_t*)b) ? 1 : (*(uint16_t*)a==*(uint16_t*)b ? 0 : -1);
}


int cmp_uint32(const void *a, const void *b)
{
    return (*(uint32_t*)a > *(uint32_t*)b) ? 1 : (*(uint32_t*)a==*(uint32_t*)b ? 0 : -1);
}


int cmp_float(const void *a, const void *b)
{
    return (*(float*)a > *(float*)b) ? 1 : (*(uint32_t*)a==*(uint32_t*)b ? 0 : -1);
}


int bytes_to_int( char *bytes, int len )
{
    int ret=0;
    char *p=(char*)&ret;

    while( len-- )
        *p++ = *bytes++;
    return ret;
}


void kalman_filter_init( kalman_filter_t *filter )
{
    memset( (void*)filter, 0, sizeof(kalman_filter_t) );
    filter->q = 0.0001f;
    filter->r = 0.1f;
    filter->p_k_k1 = 1.0f;
    filter->p_k1_k1 = 1.0f;
}


float kalman_filter_update( kalman_filter_t *filter, float newval )
{
    float val;

    filter->z_k = newval;
    if( fabsf(filter->oldval - newval) >= 1.0f )
        filter->x_k1_k1= newval*0.382f + filter->oldval*0.618f;
    else
        filter->x_k1_k1 = filter->oldval;
    filter->x_k_k1 = filter->x_k1_k1;
    filter->p_k_k1 = filter->p_k1_k1 + filter->q;
    filter->gain = filter->p_k_k1/(filter->p_k_k1 + filter->r);
    val = filter->x_k_k1 + filter->gain * (filter->z_k - filter->oldval);
    filter->p_k1_k1 = (1 - filter->gain)*filter->p_k_k1;
    filter->p_k_k1 = filter->p_k1_k1;
    filter->oldval = val;
    return val;
}

