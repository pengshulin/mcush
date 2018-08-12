#include "mcush.h"
#include "semphr.h"


//#define USE_LOCK

int running=0;
SemaphoreHandle_t *lock;

void test_shell_output(char *task)
{
    shell_write_line( task ); 
}

const char line[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
void test_spiffs(char *task)
{
    int fd;
    char fname[20];
    strcpy( fname, "/s/" );
    strcat( fname, task );
    fd = mcush_open(fname,"a+");
    if( fd )
    {
        mcush_write( fd, (char*)line, strlen(line) );
        mcush_close( fd );
    }
}


void task_test_entry(void *p)
{
    int id=(int)p;
    char buf[128];
    while( 1 )
    {
        if( running )
        {
            sprintf( buf, "t%d", id );

#if defined(USE_LOCK)
            xSemaphoreTake( lock, portMAX_DELAY );
#endif

            //test_shell_output( buf ); 
            test_spiffs( buf ); 

#if defined(USE_LOCK)
            xSemaphoreGive( lock );
#endif
            taskYIELD();
            //vTaskDelay( 1 );
            //vTaskDelay(10*configTICK_RATE_HZ/1000);
        }
        else
            vTaskDelay(100*configTICK_RATE_HZ/1000);
    }
}

int cmd_stop( int argc, char *argv[] )
{
    running = 0;
    return 0;
}

int cmd_run( int argc, char *argv[] )
{
    running = 1;
    return 0;
}

static shell_cmd_t cmd_tab[] = {
{   0, 0, "stop",  cmd_stop, 
    "stop",
    "stop"  },
{   0, 0, "run",  cmd_run, 
    "run",
    "run"  },
{   CMD_END  } };




int create_test_tasks(int num)
{
    int cnt=0;
    TaskHandle_t task;
    char buf[16];

    while( cnt < num )
    {
        sprintf( buf, "t%dT", cnt );
        xTaskCreate(task_test_entry, (const char *)buf,
                    2048 / sizeof(portSTACK_TYPE), 
                    (void*)cnt, MCUSH_PRIORITY-1, &task);
        if( !task )
            break;
        else
        {
            mcushTaskAddToRegistered((void*)task);
            cnt += 1;
        }
    } 
    return cnt;
}


int main(void)
{ 
    mcush_init();

    shell_add_cmd_table( cmd_tab );
    lock = xSemaphoreCreateMutex();
    if( !lock )
        halt("create mutex");
    create_test_tasks(10);

    mcush_start();
    while(1);
}

 
