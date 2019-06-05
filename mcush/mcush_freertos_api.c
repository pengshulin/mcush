/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <string.h>
#include <stdint.h>
#include "mcush.h"
#include "mcush_freertos_api.h"

#if MCUSH_FREERTOS_PEEK_API

#ifdef INCLUDE_MCUSH_QUEUE_API
int mcushGetQueueRegistered( int index, void **pxHandle, const char **pcQueueName )
{
    if( (index < 0) || (index >= configQUEUE_REGISTRY_SIZE) )
        return 0;

    if( xQueueRegistry[index].xHandle && xQueueRegistry[index].pcQueueName )
    {
        *pxHandle = (void*)xQueueRegistry[index].xHandle;
        *pcQueueName = xQueueRegistry[index].pcQueueName;
        return 1;
    }
	return 0;
}	


int mcushGetQueueInfo( void *pxHandle, mcush_queue_info_t *info )
{
    Queue_t *q = (Queue_t *)pxHandle;
    if( !q )
        return 0;
    info->pcHead = q->pcHead;
    info->pcTail = q->u.xQueue.pcTail;
    info->pcWriteTo = q->pcWriteTo;
    info->uxMessagesWaiting = q->uxMessagesWaiting;
    info->uxLength = q->uxLength;
    info->uxItemSize = q->uxItemSize;
    return 1;
}

#endif



#ifdef INCLUDE_MCUSH_TASK_API
    
TCB_t *mcush_task_registry[MCUSH_TASK_REGISTRY_SIZE];
uint8_t mcush_task_registry_inited;

int mcushTaskAddToRegistered( void *pxHandle )
{
    int i;
    if( !mcush_task_registry_inited )
    {
        for( i=0; i<MCUSH_TASK_REGISTRY_SIZE; i++ )
            mcush_task_registry[i] = 0;
        mcush_task_registry_inited = 1;
    }

    for( i=0; i<MCUSH_TASK_REGISTRY_SIZE; i++ )
    {
        if( mcush_task_registry[i] == (TCB_t*)pxHandle )
            return i;  /* registered */
        if( mcush_task_registry[i] == 0 )
        {
            mcush_task_registry[i] = (TCB_t*)pxHandle;
            return i;
        }
    }
#if MCUSH_HALT_ON_TASK_REGISTER_FAIL
    halt( "register task" );
#endif
    return -1;  /* failed */
}	


int mcushGetTaskInfo( int index, mcush_task_info_t *info )
{
    TCB_t *t = (TCB_t *)mcush_task_registry[index];
    int i;
    if( !t )
        return 0;

    info->pTaskHandle = (void*)t;
    info->pxTopOfStack = (uint32_t*)t->pxTopOfStack;
    info->pxStack = t->pxStack;
    info->uxPriority = t->uxPriority;
    info->pcTaskName = t->pcTaskName;
    info->uxFreeStack = 0;
    /* check free stack size */
    for( i=(int)t->pxStack; (*(int*)i == 0xa5a5a5a5) && (i<=(int)t->pxTopOfStack); i+=4 )
        info->uxFreeStack += 4;
 
    return 1;
}

void mcushGetKernInfo( mcush_kern_info_t *info )
{
    int i;
    info->uxCurrentNumberOfTasks = uxCurrentNumberOfTasks;
    info->uxTickCount = xTickCount;
    info->uxTopReadyPriority = uxTopReadyPriority;
    info->uxSchedulerRunning = xSchedulerRunning ? 1 : 0;
    info->uxPendedTicks = uxPendedTicks;
    info->uxNumOfOverflows = xNumOfOverflows;

    info->pxCurrentTCB = (void*)pxCurrentTCB;
    for( i=0; i<configMAX_PRIORITIES; i++ )
        info->pxReadyTaskLists[i] = (void*)&pxReadyTasksLists[i];
    info->pxDelayedTaskList1 = (void*)&xDelayedTaskList1;
    info->pxDelayedTaskList2 = (void*)&xDelayedTaskList2;
    info->pxDelayedTaskList = (void*)pxDelayedTaskList;
    info->pxOverflowDelayedTaskList = (void*)pxOverflowDelayedTaskList;
    info->pxPendingReadyList = (void*)&xPendingReadyList;
#if ( INCLUDE_vTaskSuspend == 1 )
    info->pxSuspendedTaskList = (void*)&xSuspendedTaskList;
#endif
}

const char *mcushGetTaskNameFromTCB( void *pxTCB )
{
    return ((TCB_t*)pxTCB)->pcTaskName;
}

char *mcushGetTaskNamesFromTaskList( void *pxTlist, char *buf )
{
    List_t *lst = pxTlist;
    ListItem_t *p = lst->pxIndex;
    MiniListItem_t *pend = &(lst->xListEnd);
    TCB_t *pTCB;
    int i;

    for(buf[0]=0, i=0; (i<lst->uxNumberOfItems) && (p!=(ListItem_t*)pend); i++)
    {
        pTCB = (TCB_t*)p->pvOwner;
        if( i )
            strcat( buf, "," );
        if( strlen(pTCB->pcTaskName) )
            strcat( buf, pTCB->pcTaskName );
        else
            strcat( buf, "?" );
        p = p->pxNext;
    }
    return buf;
}



#endif

#endif
