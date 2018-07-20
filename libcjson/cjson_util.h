#ifndef __CJSON_UTIL_H__
#define __CJSON_UTIL_H__
#include <stdint.h>

int get_uint8( cJSON *root, uint8_t *v, const char *name );
int get_uint16( cJSON *root, uint16_t *v, const char *name );
int get_uint32( cJSON *root, uint32_t *v, const char *name );
int get_uint64( cJSON *root, uint64_t *v, const char *name );
int get_float( cJSON *root, float *v, const char *name );
int get_float_array( cJSON *root, float *v, const char *name, int num );

#endif
