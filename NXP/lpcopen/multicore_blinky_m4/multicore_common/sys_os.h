#ifndef __SYS_OS_H_
#define __SYS_OS_H_

#if (defined(SYS_OS_ID) && SYS_OS_ID == 1)
#include "sys_freertos.h"
#else
#include "sys_sa.h"
#endif

#endif /* __SYS_OS_H_ */






