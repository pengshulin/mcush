/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_FREERTOS_API_H__
#define __MCUSH_FREERTOS_API_H__

#include "FreeRTOSConfig.h"

#ifndef MCUSH_FREERTOS_PEEK_API
    #define MCUSH_FREERTOS_PEEK_API  1
#endif

#ifndef MCUSH_TASK_REGISTRY_SIZE
    #define MCUSH_TASK_REGISTRY_SIZE  10
#endif


typedef struct {
    uint32_t uxCurrentNumberOfTasks;
    uint32_t uxTickCount;
    uint32_t uxTopReadyPriority;
    uint32_t uxSchedulerRunning;
    uint32_t uxPendedTicks;
    uint32_t uxNumOfOverflows;

    void *pxCurrentTCB;
    void *pxReadyTaskLists[configMAX_PRIORITIES];
    void *pxDelayedTaskList1;
    void *pxDelayedTaskList2;
    void *pxDelayedTaskList;
    void *pxOverflowDelayedTaskList;
    void *pxPendingReadyList;
#if ( INCLUDE_vTaskSuspend == 1 )
    void *pxSuspendedTaskList;
#endif
} mcush_kern_info_t;



typedef struct {
    int8_t *pcHead;
    int8_t *pcTail;
    int8_t *pcWriteTo;
    uint32_t uxMessagesWaiting;
    uint32_t uxLength;
    uint32_t uxItemSize;
} mcush_queue_info_t;

typedef struct {
    void *pTaskHandle;
    char *pcTaskName;
    uint32_t uxPriority;
    void *pxTopOfStack;
    void *pxStack;
    uint32_t uxFreeStack;
} mcush_task_info_t;


int mcushGetQueueRegistered( int index, void **pxHandle, const char **pcQueueName );
int mcushGetQueueInfo( void *xHandle, mcush_queue_info_t *info );

int mcushGetTaskInfo( void *handle, mcush_task_info_t *info );
void *mcushGetTaskStackTop( void *handle );

void mcushGetKernInfo( mcush_kern_info_t *info );

const char *mcushGetTaskNameFromTCB( void *pxTCB );
char *mcushGetTaskNamesFromTaskList( void *pxTlist, char *buf );

#endif
