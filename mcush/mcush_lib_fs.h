/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_LIB_FS_H__
#define __MCUSH_LIB_FS_H__

int mcush_file_exists( const char *fname );
int mcush_file_load_string( const char *fname, char *str, int size_limit );
int mcush_file_write_string( const char *fname, char *str );
int mcush_file_crc8( const char *fname );
int mcush_file_remove_retry( const char *fname, int retry_num );

int mcush_file_read_line( int fd, char *line );

#endif
