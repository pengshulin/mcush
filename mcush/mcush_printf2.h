/* mcush uses printf2 to replace standard sprintf from newlib
 * use macro USE_SHELL_PRINTF2=0 to disable it
 * MCUSH designed by Peng Shulin, all rights reserved. */
/*******************************************************************************
 Copyright 2013 SpinDance Inc.
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
#ifndef _PRINTF2_H
#define _PRINTF2_H
#include <stdarg.h>

#define USE_OSTREAM 0

typedef enum {
    PRINTF2_OK = 0,
    PRINTF2_WRITE_ERR = -1,
    PRINTF2_NULL_PTR = -2,
} printf2_result_t;

typedef enum {
    PRINTF2_FLAGS_NONE             = 0x0000,
    PRINTF2_FLAGS_NO_TRAILING_NULL = 0x0001,
} printf2_flags_type;

int stringff (char *out, unsigned int flags, const char *format, ...);
int stringf (char *out, const char *format, ...);

int stringffn(char *out,  unsigned int flags, unsigned int max_len, const char *format, ...);
int stringfn(char *out, unsigned int max_len, const char *format, ...);

int stringffnv(char *out, unsigned int flags, unsigned int max_len, const char *format, va_list vargs);
int stringfnv(char *out, unsigned int max_len, const char *format, va_list vargs);

#endif
