/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_BEEP_H__
#define __TASK_BEEP_H__

#define TASK_BEEP_STACK_SIZE    352
#define TASK_BEEP_PRIORITY      OS_PRIORITY_NORMAL
#define TASK_BEEP_QUEUE_SIZE    1

#define BEEP_EVENT_RESET        0
#define BEEP_EVENT_TONE         1
#define BEEP_EVENT_NOTE         2
#define BEEP_EVENT_LIST         3


typedef struct {
    uint16_t freq;
    uint16_t ms;
} beep_note_t;

typedef struct {
    uint8_t type;
    uint8_t flag;
    union {
        beep_note_t val;
        const beep_note_t *p;
    } note;
    uint16_t repeat_count;
    uint16_t repeat_delay_ms;
} beep_event_t;


void task_beep_init(void);
void task_beep_entry(void *p);

int beep_note( const beep_note_t *note, int repeat_count, int repeat_delay_ms );
int beep_notes( const beep_note_t *note );


/* preset notes */
extern const beep_note_t beep_note_1000_50;
extern const beep_note_t beep_note_1000_200;
extern const beep_note_t beep_note_1000_500;
extern const beep_note_t beep_note_1000_1000;
extern const beep_note_t beep_note_4000_50;
extern const beep_note_t beep_note_4000_100;
extern const beep_note_t beep_note_4000_500;
extern const beep_note_t beep_note_4000_1000;

extern const beep_note_t beep_notes_poweron[];
extern const beep_note_t beep_notes_poweroff[];
extern const beep_note_t beep_notes_succ[];
extern const beep_note_t beep_notes_fail[];


#endif
