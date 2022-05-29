/* LVGL task based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_logger.h"
#include "task_lvgl.h"
#include <lvgl.h>
#include "lv_demos.h"
#include "lv_examples.h"


LOGGER_MODULE_NAME("lvgl");


extern int hal_lvgl_init(void);



void task_lvgl_entry(void *arg)
{
    (void)arg;
  
    lv_init();

    if( hal_lvgl_init() == 0 )
        halt("hal_lvgl_init");


    //lv_example_anim_timeline_1();
    //lv_example_btn_1();
    //lv_example_btnmatrix_2();
    //lv_example_calendar_1();
    //lv_example_canvas_2();
    //lv_example_chart_6();
    //lv_example_checkbox_1();
    //lv_example_colorwheel_1();
    //lv_example_dropdown_2();
    //lv_example_event_4();
    //lv_example_flex_6();
    //lv_example_get_started_3();
    //lv_example_grid_3();
    //lv_example_label_5();
    //lv_example_led_1();
    //lv_example_line_1();
    //lv_example_menu_5();
    //lv_example_meter_4();
    //lv_example_msgbox_1();
    //lv_example_roller_3();
    //lv_example_scroll_2();
    //lv_example_span_1();
    //lv_example_style_14();
    //lv_example_switch_1();
    //lv_example_table_2();
    //lv_example_tabview_2();
    //lv_example_textarea_3();
    //lv_example_tileview_1();
    //lv_example_win_1();

    //lv_demo_benchmark();
    //lv_demo_stress();
    //lv_demo_music();
    //lv_demo_keypad_encoder();
    lv_demo_widgets();

    while( 1 )
    {
        lv_timer_handler();
        os_task_delay_ms(LV_DISP_DEF_REFR_PERIOD);
    }
}


void task_lvgl_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( lvgl, TASK_LVGL_STACK_SIZE );
    task = os_task_create_static( "lvglT", task_lvgl_entry, NULL,
                TASK_LVGL_STACK_SIZE, TASK_LVGL_PRIORITY,
                &static_task_buffer_lvgl );
#else
    task = os_task_create( "lvglT", task_lvgl_entry, NULL,
                TASK_LVGL_STACK_SIZE, TASK_LVGL_PRIORITY );
#endif
    if( task == NULL )
        halt("create lvgl task");
}

