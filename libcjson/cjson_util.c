/* cjson utilities
 * Peng Shulin <trees_peng@163.com> 2017
 */
#include <stdint.h>
#include "cJSON.h"
 
int get_uint8( cJSON *root, uint8_t *v, const char *name )
{
    cJSON *item = cJSON_GetObjectItem( root, name );
    if( item && cJSON_IsNumber(item) )
    {
        *v = item->valueint;
        return 1;
    }
    return 0;
} 


int get_uint16( cJSON *root, uint16_t *v, const char *name )
{
    cJSON *item = cJSON_GetObjectItem( root, name );
    if( item && cJSON_IsNumber(item) )
    {
        *v = item->valueint;
        return 1;
    }
    return 0;
} 


int get_uint32( cJSON *root, uint32_t *v, const char *name )
{
    cJSON *item = cJSON_GetObjectItem( root, name );
    if( item && cJSON_IsNumber(item) )
    {
        *v = item->valueint;
        return 1;
    }
    return 0;
}           


int get_uint64( cJSON *root, uint64_t *v, const char *name )
{
    cJSON *item = cJSON_GetObjectItem( root, name );
    if( item && cJSON_IsNumber(item) )
    {
        *v = (uint64_t)item->valuedouble;
        return 1;
    }
    return 0;
}


int get_float( cJSON *root, float *v, const char *name )
{
    cJSON *item = cJSON_GetObjectItem( root, name );
    if( item && cJSON_IsNumber(item) )
    {
        *v = (float)item->valuedouble;
        return 1;
    }
    return 0;
}


int get_float_array( cJSON *root, float *v, const char *name, int num )
{
    cJSON *subroot = cJSON_GetObjectItem( root, name ), *item;
    if( !subroot || !cJSON_IsArray(subroot) )
        return 0;
    item = subroot->child;
    if( !item )
        return 0;
    while( num-- )
    {
        if( item && cJSON_IsNumber(item) )
            *v++ = (float)item->valuedouble;
        else
            return 0;
        item = item->next;
    }
    return 1;
}



