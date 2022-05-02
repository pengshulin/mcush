/* Beeper service task 
 * control the beeper to play single-tone note or notes list
 *
 * The task runs as an auxiliary service in MCUSH micro-kerenl RTOS environment,
 * it aims to help you debug your applications.
 * To use this service task, include task_beep.h file in your application
 * and call task_beep_init() before you use it.
 * 
 * MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_beep.h"


/* preset note(s): */
  
/* 1000 Hz */
const beep_note_t beep_note_1000_50 = { 1000, 50 };
const beep_note_t beep_note_1000_200 = { 1000, 200 };
const beep_note_t beep_note_1000_500 = { 1000, 500 };
const beep_note_t beep_note_1000_1000 = { 1000, 1000 };
/* 4000 Hz */
const beep_note_t beep_note_4000_50 = { 4000, 50 };
const beep_note_t beep_note_4000_100 = { 4000, 100 };
const beep_note_t beep_note_4000_500 = { 4000, 500 };
const beep_note_t beep_note_4000_1000 = { 4000, 1000 };
/* notes list: */
const beep_note_t beep_notes_poweron[] = { {195, 62}, {783, 125}, {391, 62}, {1567, 125}, {0, 0} };
const beep_note_t beep_notes_poweroff[] = { {1567, 62}, {783, 125}, {391, 62}, {195, 125}, {0, 0} };
const beep_note_t beep_notes_succ[] = { {1000, 200}, {4000, 200}, {0, 0} };
const beep_note_t beep_notes_fail[] = { {4000, 200}, {1000, 200}, {0, 0} };



static os_queue_handle_t queue_beep;


int beep_note( const beep_note_t *note, int repeat_count, int repeat_delay_ms )
{
    beep_event_t evt;
    
    evt.type = BEEP_EVENT_NOTE;
    evt.note.val.freq = note->freq;
    evt.note.val.ms = note->ms;
    evt.repeat_count = repeat_count;
    evt.repeat_delay_ms = repeat_delay_ms;
    return os_queue_put( queue_beep, &evt, 0 );
}


int beep_notes( const beep_note_t *note )
{
    beep_event_t evt;

    evt.type = BEEP_EVENT_LIST;
    evt.note.p = note;
    return os_queue_put( queue_beep, &evt, 0 );
}


void task_beep_entry(void *p)
{
    beep_event_t evt;
    const beep_note_t *note;

    (void)p;

    while( 1 )
    {
        if( os_queue_get( queue_beep, &evt, -1 ) == 0 )
            continue;

        switch( evt.type )
        {
        case BEEP_EVENT_RESET:
            hal_beep_off();
            break;

        case BEEP_EVENT_TONE:
            hal_beep_on( evt.note.val.freq ); 
            break;

        case BEEP_EVENT_NOTE:
            while(1)
            {
                hal_beep_on( evt.note.val.freq ); 
                os_task_delay_ms( evt.note.val.ms );
                hal_beep_off();
                if( evt.repeat_count == 0 )
                    break;
                os_task_delay_ms( evt.repeat_delay_ms );
                evt.repeat_count--;
            }
            break;

        case BEEP_EVENT_LIST:
            note = evt.note.p;
            while( note->freq || note->ms )
            {
                hal_beep_on( note->freq ); 
                os_task_delay_ms( note->ms );
                hal_beep_off();
                note++; 
            }
            break;

        default:
            os_task_delay_ms( 100 );
        }
    }
}


void task_beep_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( beep, TASK_BEEP_STACK_SIZE );
    task = os_task_create_static( "beepT", task_beep_entry, 0, TASK_BEEP_STACK_SIZE, TASK_BEEP_PRIORITY, &static_task_buffer_beep );
#else
    task = os_task_create( "beepT", task_beep_entry, 0, TASK_BEEP_STACK_SIZE, TASK_BEEP_PRIORITY );
#endif
    if( task == NULL )
        halt("create beep task");

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_QUEUE_BUFFER( queue_beep, TASK_BEEP_QUEUE_SIZE, sizeof(beep_event_t) );
    queue_beep = os_queue_create_static( "beepQ", TASK_BEEP_QUEUE_SIZE, sizeof(beep_event_t),
                                           &static_queue_buffer_queue_beep );
#else
    queue_beep = os_queue_create_static( "beepQ", TASK_BEEP_QUEUE_SIZE, sizeof(beep_event_t) );
#endif
    if( queue_beep == NULL )
        halt("create beep queue");
}

