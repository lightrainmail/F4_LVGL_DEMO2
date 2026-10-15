//
// Created by LightRain on 2026/10/13.
//
#include "ui.h"


static void button_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
}

void create_button(void)
{
    /* 在当前屏幕上创建按钮 */
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_center(btn);

    /* 添加按钮文字 */
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me");
    lv_obj_center(label);

    /* 注册点击事件 */
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, NULL);
}