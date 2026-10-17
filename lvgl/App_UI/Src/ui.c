//
// Created by LightRain on 2026/10/13.
//
#include "ui.h"
#include "lvgl.h"
#include "flash_storage.h"

static uint32_t click_count = 0;

static void test_button_event_cb(lv_event_t *e)
{
    static uint8_t step = 0;

    static uint8_t tx[300];
    static uint8_t rx[300];

    const uint32_t address = 0xFFF0F0UL;

    lv_obj_t *label = lv_event_get_user_data(e);

    /* 第一次点击：生成数据并写入 */
    if (step == 0U) {
        for (uint32_t i = 0; i < sizeof(tx); ++i) {
            tx[i] = (uint8_t)i;
        }

        /* 先标记结束，防止失败后重复编程 */
        step = 2U;

        if (!w25q128_program(address, tx, sizeof(tx))) {
            lv_label_set_text(label, "Program Failed");
            return;
        }

        step = 1U;

        lv_label_set_text(label,
                          "Program Done\n"
                          "Click to verify");
        return;
    }

    /* 第二次点击：读回并比较 */
    if (step == 1U) {
        if (!w25q128_read(address, rx, sizeof(rx))) {
            lv_label_set_text(label,
                              "Read Failed\n"
                              "Click to retry");
            return;
        }

        for (uint32_t i = 0; i < sizeof(tx); ++i) {
            if (rx[i] != tx[i]) {
                lv_label_set_text_fmt(
                    label,
                    "Verify Failed\n"
                    "Offset: %lu\n"
                    "TX: %02X RX: %02X",
                    (unsigned long)i,
                    (unsigned int)tx[i],
                    (unsigned int)rx[i]);

                step = 2U;
                return;
            }
        }

        step = 2U;

        lv_label_set_text(label,
                          "Cross Page PASS\n"
                          "300 bytes matched");
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

/*
 * 返回值：
 * 0：测试通过
 * 1：擦除失败
 * 2：写入失败
 * 3：首次读取失败
 * 4：首次比较不一致
 * 5：更新失败
 * 6：更新后读取失败
 * 7：更新后比较不一致
 */
uint8_t flash_storage_test(void)
{
    const uint32_t address = 0xFFF000UL;

    uint8_t original[8] = {
        0x11, 0x22, 0x00, 0x00,
        0x55, 0x66, 0x77, 0x88
    };

    uint8_t new_data[2] = {0xFF, 0xFF};

    uint8_t expected[8] = {
        0x11, 0x22, 0xFF, 0xFF,
        0x55, 0x66, 0x77, 0x88
    };

    uint8_t actual[8];

    /* 1. 擦除 */
    if (!flash_erase_sector(address)) {
        return 1U;
    }

    /* 2. 写入原始数据 */
    if (!flash_program(address, original, sizeof(original))) {
        return 2U;
    }

    /* 3. 读取原始数据 */
    if (!flash_read(address, actual, sizeof(actual))) {
        return 3U;
    }

    /* 4. 比较原始数据 */
    for (uint32_t i = 0; i < sizeof(actual); ++i) {
        if (actual[i] != original[i]) {
            return 4U;
        }
    }

    /* 5. 更新偏移 2、3 的两个字节 */
    if (!flash_update(address + 2U, new_data, sizeof(new_data))) {
        return 5U;
    }

    /* 6. 再次读取 */
    if (!flash_read(address, actual, sizeof(actual))) {
        return 6U;
    }

    /* 7. 确认更新成功，且相邻数据保持不变 */
    for (uint32_t i = 0; i < sizeof(actual); ++i) {
        if (actual[i] != expected[i]) {
            return 7U;
        }
    }

    return 0U;
}
