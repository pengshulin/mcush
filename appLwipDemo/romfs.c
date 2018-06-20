/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

const char file_readme[] = "DHCP client and httpd demo based on LWIP/MCUSH\nPeng Shulin <trees_peng@163.com> 2018\n";

const romfs_file_t romfs_tab[] = {
    { "readme", file_readme, sizeof(file_readme)-1 },
    { 0 } };

