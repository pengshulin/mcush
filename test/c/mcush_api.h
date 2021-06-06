#ifndef __MCUSH_API_H__
#define __MCUSH_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MCUSH_TERMINATOR_RESET
    #define MCUSH_TERMINATOR_RESET  '\x03'
#endif

#ifndef MCUSH_TERMINATOR_WRITE
    #define MCUSH_TERMINATOR_WRITE  '\x0A'
#endif

#ifndef MCUSH_TERMINATOR_READ
    #define MCUSH_TERMINATOR_READ  '\x0A'
#endif

#ifndef MCUSH_DEFAULT_BAUDRATE
    #define MCUSH_DEFAULT_BAUDRATE  9600
#endif

#ifndef MCUSH_ALLOC_BYTES
    #define MCUSH_ALLOC_BYTES  512
#endif


#define MCUSH_ERR_API                   -10000
#define MCUSH_ERR_IO                    -10001
#define MCUSH_ERR_MEMORY                -10002
#define MCUSH_ERR_TIMEOUT               -10003
#define MCUSH_ERR_SYNTAX                -10004
#define MCUSH_ERR_EXEC                  -10005
#define MCUSH_ERR_RET_CMD_NOT_MATCH     -10006


#define MCUSH_PROMPT_TYPE_NORMAL        0
#define MCUSH_PROMPT_TYPE_SYNTAX_ERR    1
#define MCUSH_PROMPT_TYPE_EXEC_ERR      2


typedef struct {
    const char *ttyname;
#ifdef WIN32
    void *handle;
#else
    int handle;
#endif
    char prompt[4];
    int prompt_type;
    char *response;
    int response_alloc_bytes;
    char *result;
} mcush_dev_t;

int mcush_open( mcush_dev_t *device, const char *ttyname, int baudrate );
int mcush_close( mcush_dev_t *device );
int mcush_connect( mcush_dev_t *device );
int mcush_wait_until_prompts( mcush_dev_t *device );
int mcush_write_command( mcush_dev_t *device, const char *cmd );

int mcush_putc( mcush_dev_t *device, char c );
int mcush_puts( mcush_dev_t *device, const char *str );
int mcush_getc( mcush_dev_t *device, char *c );

int mcush_scpi_idn( mcush_dev_t *device, char *output );
int mcush_scpi_rst( mcush_dev_t *device );


#ifdef __cplusplus
}
#endif

#endif
