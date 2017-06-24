/* mcush uses printf2 to replace standard sprintf from newlib
 * use macro USE_SHELL_PRINTF2=0 to disable it
 * MCUSH designed by Peng Shulin, all rights reserved. */
/*******************************************************************************
 Copyright 2001, 2002 Georges Menie (<URL snipped>)
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*******************************************************************************
 putchar is the only external dependency for this file, 
 if you have a working putchar, just remove the following
 define. If the function should be called something else,
 replace outbyte(c) by your own function call.
 */
//*******************************************************************************
//  Updated by Daniel D Miller.  Changes to the original Menie code are
//  Copyright 2009-2013 Daniel D Miller
//  All such changes are distributed under the same license as the original,
//  as described above.
//  11/06/09 - adding floating-point support
//  03/19/10 - pad fractional portion of floating-point number with 0s
//  03/30/10 - document the \% bug
//  07/20/10 - Fix a round-off bug in floating-point conversions
//             ( 0.99 with %.1f did not round to 1.0 )
//  10/25/11 - Add support for %+ format (always show + on positive numbers)
//  05/10/13 - Add stringfn() function, which takes a maximum-output-buffer
//             length as an argument.  Similar to snprintf()
//*******************************************************************************
//  BUGS
//  If '%' is included in a format string, in the form \% with the intent
//  of including the percent sign in the output string, this function will
//  mis-handle the data entirely!!  
//  Essentially, it will just discard the character following the percent sign.  
//  This bug is not easy to fix in the existing code; 
//  for now, I'll just try to remember to use %% instead of \% ...
//*******************************************************************************
//  compile command for stand-alone mode (TEST_PRINTF)
//  gcc -Wall -O2 -DTEST_PRINTF -s printf2.c -o printf2.exe
//*******************************************************************************
//*******************************************************************************
//  Updated by SpinDance Inc.  Changes to the original Menie code are
//  Copyright 2013-2014 SpinDance Inc.
//  All such changes are distributed under the same license as the original,
//  as described above.
//  09/07/13 - Removed references to USE_INTERNALS.
//  09/07/13 - Use a standard strlen.
//  09/18/13 - Added stringfnp().
//  01/28/14 - Fixed bug where %s strings overflowed buffer with final '\0'.
//  01/28/14 - Fixed linux i686 gcc double stack alignment bug.
//  01/28/14 - Improved testing support.
//  01/28/14 - Uses stdarg.h - runs optimized with arm-none-eabi-gcc.
//  08/17/14 - Add support for nanopb streams.
//*******************************************************************************

//lint -esym(752, debug_output)
//lint -esym(766, stdio.h)

#include <stdint.h>
#include "shell.h"
#if USE_SHELL_PRINTF2
#include "mcush_printf2.h"
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#if USE_FLOATING_POINT
#define FLOAT_OR_DOUBLE float
#else
#define FLOAT_OR_DOUBLE double
#endif

//lint -e534  Ignoring return value of function 
//lint -e539  Did not expect positive indentation from line ...
//lint -e525  Negative indentation from line ...

typedef  unsigned char  u8 ;
typedef  unsigned int   uint ;



//****************************************************************************
static int printchar (
    char **str,
    int c,
    unsigned int max_output_len,
    int *cur_output_char_p)
{
    if (max_output_len >= 0  &&  *cur_output_char_p >= max_output_len)
        return PRINTF2_OK;

    if (str) {
        **str = (char) c;
        ++(*str);
        (*cur_output_char_p)++ ;
    }
    else return PRINTF2_NULL_PTR;

    return PRINTF2_OK;
}

//****************************************************************************
//  This version returns the length of the output string.
//  It is more useful when implementing a walking-string function.
//****************************************************************************
//lint -esym(728, round_nums)
//static const FLOAT_OR_DOUBLE round_nums[8] = {
static const FLOAT_OR_DOUBLE round_nums[] = {
        0.5,
        0.05,
        0.005,
        0.0005,
        0.00005,
        0.000005,
        0.0000005,
        0.00000005,
#if ! USE_FLOATING_POINT
        0.000000005,
        0.0000000005,
        0.00000000005,
        0.000000000005,
        0.0000000000005,
        0.00000000000005,
        0.000000000000005,
#endif
} ;

static unsigned fltordbl2stri(char *outbfr, FLOAT_OR_DOUBLE flt_or_dbl, unsigned dec_digits, int use_leading_plus)
{
    static char local_bfr[128] ;
    char *output = (outbfr == 0) ? local_bfr : outbfr ;

    //*******************************************
    //  extract negative info
    //*******************************************
    if (flt_or_dbl < 0.0) {
        *output++ = '-' ;
        flt_or_dbl *= -1.0 ;
    } else {
        if (use_leading_plus) {
            *output++ = '+' ;
        }

    }

    //  handling rounding by adding .5LSB to the floating-point data
    if (dec_digits < 8) {
        flt_or_dbl += round_nums[dec_digits] ;
    }

    //**************************************************************************
    //  construct fractional multiplier for specified number of digits.
    //**************************************************************************
    uint mult = 1 ;
    uint idx ;
    for (idx=0; idx < dec_digits; idx++)
        mult *= 10 ;

    // printf("mult=%u\n", mult) ;
    uint wholeNum = (uint) flt_or_dbl ;
    uint decimalNum = (uint) ((flt_or_dbl - wholeNum) * mult);

    //*******************************************
    //  convert integer portion
    //*******************************************
    char tbfr[40] ;
    idx = 0 ;
    while (wholeNum != 0) {
        tbfr[idx++] = '0' + (wholeNum % 10) ;
        wholeNum /= 10 ;
    }
    // printf("%.3f: whole=%s, dec=%d\n", dbl, tbfr, decimalNum) ;
    if (idx == 0) {
        *output++ = '0' ;
    } else {
        while (idx > 0) {
            *output++ = tbfr[idx-1] ;  //lint !e771
            idx-- ;
        }
    }
    if (dec_digits > 0) {
        *output++ = '.' ;

        //*******************************************
        //  convert fractional portion
        //*******************************************
        idx = 0 ;
        while (decimalNum != 0) {
            tbfr[idx++] = '0' + (decimalNum % 10) ;
            decimalNum /= 10 ;
        }
        //  pad the decimal portion with 0s as necessary;
        //  We wouldn't want to report 3.093 as 3.93, would we??
        while (idx < dec_digits) {
            tbfr[idx++] = '0' ;
        }
        // printf("decimal=%s\n", tbfr) ;
        if (idx == 0) {
            *output++ = '0' ;
        } else {
            while (idx > 0) {
                *output++ = tbfr[idx-1] ;
                idx-- ;
            }
        }
    }
    *output = 0 ;

    //  prepare output
    output = (outbfr == 0) ? local_bfr : outbfr ;

    return strlen(output) ;
}

//****************************************************************************
#define  PAD_RIGHT   1
#define  PAD_ZERO    2

static int prints (
                     char **out,
                     const char *string, int width, int pad,
                     unsigned int max_output_len, int *cur_output_char_p)
{
    register int pc = 0, padchar = ' ';
    if (width > 0) {
        int len = 0;
        const char *ptr;
        for (ptr = string; *ptr; ++ptr)
            ++len;
        if (len >= width)
            width = 0;
        else
            width -= len;
        if (pad & PAD_ZERO)
            padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for (; width > 0; --width) {
            int result = printchar (
                       out,
                       padchar, max_output_len, cur_output_char_p);
            if (result <0) return result;
            ++pc;
        }
    }
    for (; *string; ++string) {
        int result = printchar (
                   out,
                   *string, max_output_len, cur_output_char_p);
        if (result < 0) return result;
        ++pc;
    }
    for (; width > 0; --width) {
        int result = printchar (
                   out,
                   padchar, max_output_len,cur_output_char_p);
        if (result < 0) return result;
        ++pc;
    }
    return pc;
}

//****************************************************************************
/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12
static int printi (
                     char **out,
                     int i, uint base, int sign, int width, int pad,
                     int letbase, unsigned int max_output_len,
                     int *cur_output_char_p, int use_leading_plus)
{
    char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned u = (unsigned) i;
    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (
                        out,
                        print_buf, width, pad, max_output_len, cur_output_char_p);
    }
    if (sign && base == 10 && i < 0) {
        neg = 1;
        u = (unsigned) -i;
    }
    //  make sure print_buf is NULL-term
    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u) {
        t = u % base;  //lint !e573 !e737 !e713 Warning 573: Signed-unsigned mix with divide
        if (t >= 10)
            t += letbase - '0' - 10;
        *--s = (char) t + '0';
        u /= base;  //lint !e573 !e737  Signed-unsigned mix with divide
    }
    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            int result = printchar (
                       out,
                       '-', max_output_len, cur_output_char_p);
            if (result < 0) return result;
            ++pc;
            --width;
        }
        else {
            *--s = '-';
        }
    } else {
        if (use_leading_plus) {
            *--s = '+';
        }
    }
    int result = prints (
                         out,
                         s, width, pad, max_output_len, cur_output_char_p);
    if (result <0) {
        return result;
    } else {
        return pc + result;
    }
}

//****************************************************************************
static int print (
    char **out,
    unsigned int flags,
    unsigned int max_output_len,
    const char *format,
    va_list vargs)
{
    int post_decimal ;
    int width, pad ;
    unsigned dec_width = 6 ;
    int pc = 0;
    char scr[2];
    int cur_output_char = 0;
    int *cur_output_char_p = &cur_output_char;
    int use_leading_plus = 0 ;  //  start out with this clear

    max_output_len--; // make room for a trailing '\0'
    for (; *format != 0; ++format) {
        if (*format == '%') {
            dec_width = 6 ;
            ++format;
            width = pad = 0;
            if (*format == '\0')
                break;
            if (*format == '%')
                goto out_lbl;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            if (*format == '+') {
                ++format;
                use_leading_plus = 1 ;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            post_decimal = 0 ;
            if (*format == '.'  ||
                    (*format >= '0' &&  *format <= '9')) {

                while (1) {
                    if (*format == '.') {
                        post_decimal = 1 ;
                        dec_width = 0 ;
                        format++ ;
                    } else if ((*format >= '0' &&  *format <= '9')) {
                        if (post_decimal) {
                            dec_width *= 10;
                            dec_width += (uint) (u8) (*format - '0');
                        } else {
                            width *= 10;
                            width += *format - '0';
                        }
                        format++ ;
                    } else {
                        break;
                    }
                }
            }
            if (*format == 'l')
                ++format;
            if (*format == 'h')
                ++format;
            switch (*format) {
            case 's':
            {
                char *s = va_arg(vargs, char*);
                int result = prints (
                    out,
                    s ? s : "(null)", width, pad, max_output_len, cur_output_char_p);
                if (result<0) return result;
                pc += result;
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;
            case 'd':
            {
                int result = printi (
                        out,
                        va_arg(vargs, int), 10, 1, width, pad, 'a', max_output_len, cur_output_char_p, use_leading_plus);
                if (result<0) return result;
                pc += result;

                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;
            case 'x':
            {
                int result = printi (
                        out,
                        va_arg(vargs, int), 16, 0, width, pad, 'a', max_output_len, cur_output_char_p, use_leading_plus);
                if (result<0) return result;
                pc += result;
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;
            case 'X':
            {
                int result = printi (
                        out,
                        va_arg(vargs, int), 16, 0, width, pad, 'A', max_output_len, cur_output_char_p, use_leading_plus);
                if (result<0) return result;
                pc += result;
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;
            case 'p':
            case 'u':
            {
                int result = printi (
                              out,
                              va_arg(vargs, int), 10, 0, width, pad, 'a', max_output_len, cur_output_char_p, use_leading_plus);
                use_leading_plus = 0 ;  //  reset this flag after printing one value
                if (result<0) return result;
                pc += result;
            }
            break;
            case 'c':
            {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char)va_arg(vargs,int);
                scr[1] = '\0';
                int result = prints (
                              out,
                              scr, width, pad, max_output_len, cur_output_char_p);
                if (result<0) return result;
                pc += result;
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;

            case 'f':
            {
                double d = va_arg(vargs,double);
                FLOAT_OR_DOUBLE flt_or_dbl = d;
                char bfr[81];
                fltordbl2stri(bfr, flt_or_dbl, dec_width, use_leading_plus);
                int result = prints (
                              out,
                              bfr, width, pad, max_output_len, cur_output_char_p);
                if (result<0) return result;
                pc += result;
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;

            default: {
                int result = printchar (
                           out,
                           '%', max_output_len, cur_output_char_p);
                if (result<0) return result;
                ++pc;

                result = printchar (
                           out,
                           *format, max_output_len, cur_output_char_p);
                if (result<0) return result;
                ++pc;

                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
                break;
            }
        } else {
            out_lbl: {
                int result = printchar (
                           out,
                           *format, max_output_len, cur_output_char_p);
                if (result<0) return result;
                ++pc;
            }
        }
    }  //  for each char in format string

    max_output_len++; // make room for a trailing '\0'

    if (!(flags & PRINTF2_FLAGS_NO_TRAILING_NULL)) {
        if (out) //lint !e850
            **out = '\0';
        else
            return PRINTF2_NULL_PTR;
    }

    return pc;
}

//****************************************************************************
int stringff (char *out, unsigned int flags, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);
    int result = print (
                        &out,
                        flags,
                        UINT_MAX, format, vargs);
    va_end(vargs);
    return result;
}


//****************************************************************************
int stringf (char *out, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);
    int result = print (
                        &out,
                        PRINTF2_FLAGS_NONE,
                        UINT_MAX, format, vargs);
    va_end(vargs);
    return result;
}


//****************************************************************************
int stringffn(char *out,  unsigned int flags, unsigned int max_len, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);
    int result = print (
                        &out,
                        flags,
                        max_len, format, vargs);
    va_end(vargs);
    return result;
}


//****************************************************************************
//lint -esym(714, stringfn)
//lint -esym(759, stringfn)
//lint -esym(765, stringfn)
int stringfn(char *out, unsigned int max_len, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);
    int result = print (
                        &out,
                        PRINTF2_FLAGS_NONE,
                        max_len, format, vargs);
    va_end(vargs);
    return result;
}


//****************************************************************************
int stringffnv(char *out, unsigned int flags, unsigned int max_len, const char *format, va_list vargs)
{
    return print (
                   &out,
                   flags,
                   max_len,
                   format,
                   vargs
    );
}

//****************************************************************************
int stringfnv(char *out, unsigned int max_len, const char *format, va_list vargs)
{
    return print (
                   &out,
                   PRINTF2_FLAGS_NONE,
                   max_len,
                   format,
                   vargs
    );
}


//****************************************************************************
int snprintf(char *str, size_t maxLen, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);

    int charCnt = print(
                        &str,
                        PRINTF2_FLAGS_NONE,
                        (unsigned int)maxLen,
                        format,
                        vargs
    );

    va_end(vargs);

    return charCnt;
}

//****************************************************************************
int sprintf(char *str, const char *format, ...)
{
    va_list vargs;
    va_start(vargs,format);

    int charCnt = print(
                        &str,
                        PRINTF2_FLAGS_NONE,
                        UINT_MAX,
                        format,
                        vargs
    );

    va_end(vargs);

    return charCnt;
}


/* called by shell_printf, replace the standard lib */
int vsprintf(char *str, const char *format, va_list ap)
{
    return print(
        &str,
        PRINTF2_FLAGS_NONE,
        UINT_MAX,
        format,
        ap );
}


#endif
