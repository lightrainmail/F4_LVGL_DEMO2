//
// Created by LightRain on 2026/10/13.
//
#include "ui.h"

#include "lvgl.h"

static uint32_t click_count = 0;

static void test_button_event_cb(lv_event_t *e)
{
    static uint32_t offset = 0;

    lv_obj_t *label = lv_event_get_user_data(e);
    uint8_t data[256];
    uint32_t address = 0xFFF000UL + offset;

    if (!w25q128_read(address, data, sizeof(data))) {
        lv_label_set_text_fmt(
            label,
            "Read Failed\nAddr: %06lX",
            (unsigned long)address);
        return;
    }

    for (uint32_t i = 0; i < sizeof(data); ++i) {
        if (data[i] != 0xFFU) {
            lv_label_set_text_fmt(
                label,
                "Not FF\nAddr: %06lX\nData: %02X",
                (unsigned long)(address + i),
                (unsigned int)data[i]);
            return;
        }
    }

    offset += sizeof(data);

    if (offset >= W25Q128_SECTOR_SIZE) {
        lv_label_set_text(label,
                          "Check PASS\n4096 bytes = FF");
        offset = 0;
    } else {
        lv_label_set_text_fmt(
            label,
            "Block OK\nChecked: %lu / 4096",
            (unsigned long)offset);
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