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


lv_obj_t * ui_loading_create(lv_obj_t * parent)
{
    lv_obj_t * spinner = lv_spinner_create(parent);

    /* 大小、位置 */
    lv_obj_set_size(spinner, 60, 60);
    lv_obj_center(spinner);

    /* 动画周期 1000 ms，弧长参数 90° */
    lv_spinner_set_anim_params(spinner, 1000, 90);

    /* 底圈：深灰色 */
    lv_obj_set_style_arc_color(
        spinner, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 6, LV_PART_MAIN);

    /* 旋转弧：蓝色、圆角 */
    lv_obj_set_style_arc_color(
        spinner, lv_color_hex(0x2196F3), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(spinner, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(
        spinner, true, LV_PART_INDICATOR);

    return spinner;
}