//
// Created by LightRain on 2026/10/13.
//
#include "ui.h"

#include "lvgl.h"

static uint32_t click_count = 0;

static void test_button_event_cb(lv_event_t *e)
{
    static bool attempted = false;

    lv_obj_t *label = lv_event_get_user_data(e);

    uint8_t tx[4] = {0x12, 0x34, 0x56, 0x78};
    uint8_t rx[4] = {0};

    /* 本次运行只尝试写入一次 */
    if (attempted) {
        return;
    }

    attempted = true;

    if (!w25q128_page_program(0xFFF000UL, tx, sizeof(tx))) {
        lv_label_set_text(label, "Program Failed");
        return;
    }

    if (!w25q128_read(0xFFF000UL, rx, sizeof(rx))) {
        lv_label_set_text(label, "Read Failed");
        return;
    }

    bool match = true;

    for (uint32_t i = 0; i < sizeof(tx); ++i) {
        if (rx[i] != tx[i]) {
            match = false;
            break;
        }
    }

    if (match) {
        lv_label_set_text(label,
                          "Program PASS\n12 34 56 78");
    } else {
        lv_label_set_text_fmt(
            label,
            "Verify Failed\n%02X %02X %02X %02X",
            (unsigned int)rx[0],
            (unsigned int)rx[1],
            (unsigned int)rx[2],
            (unsigned int)rx[3]);
    }
}

/* 创建测试界面：初始化完成后调用一次 */
void ui_button_test_create(void)
{
    lv_obj_t *screen = lv_screen_active();

    click_count = 0;

    /* 创建计数标签 */
    lv_obj_t *count_label = lv_label_create(screen);

    lv_label_set_text(count_label, "Count: 0");
    lv_obj_align(count_label, LV_ALIGN_CENTER, 0, -50);

    /* 创建按钮：LVGL V9 使用 lv_button_create */
    lv_obj_t *button = lv_button_create(screen);

    lv_obj_set_size(button, 140, 60);
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 30);

    /* 创建按钮文字 */
    lv_obj_t *button_label = lv_label_create(button);

    lv_label_set_text(button_label, "Click Me");
    lv_obj_center(button_label);

    /* 注册点击事件，把计数标签作为用户数据传入 */
    lv_obj_add_event_cb(
        button,
        test_button_event_cb,
        LV_EVENT_CLICKED,
        count_label);
}