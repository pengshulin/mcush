/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

const char file_readme[] = "Modbus/TCP/RTU demo based on MCUSH\nPeng Shulin <trees_peng@163.com> 2019\n";

const romfs_file_t romfs_tab[] = {
    { "readme", file_readme, sizeof(file_readme)-1 },
    { 0 } };

