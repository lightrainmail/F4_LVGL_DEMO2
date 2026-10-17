//
// Created by LightRain on 2026/10/13.
//
#include "ui.h"

#include "lvgl.h"

/* 校验失败时，在调试器中查看这三个变量 */
uint32_t flash_test_bad_address;
uint8_t flash_test_expected_value;
uint8_t flash_test_actual_value;

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

flash_update_test_result_t flash_update_test(void)
{
    uint8_t new_data[2] = {0xFF, 0xFF};

    flash_test_bad_address = 0;
    flash_test_expected_value = 0;
    flash_test_actual_value = 0;

    /* 1. 生成整个扇区的测试数据 */
    for (uint32_t i = 0; i < W25Q128_SECTOR_SIZE; ++i) {
        test_expected[i] = (uint8_t)i;
    }

    /* 目标位置明确设为 00，之后测试更新成 FF */
    test_expected[16] = 0x00;
    test_expected[17] = 0x00;

    /* 2. 擦除测试扇区 */
    if (!w25q128_erase_sector(FLASH_TEST_ADDRESS)) {
        return FLASH_UPDATE_TEST_ERASE_FAILED;
    }

    /* 3. 写入原始测试数据 */
    if (!w25q128_program(FLASH_TEST_ADDRESS,
                         test_expected,
                         W25Q128_SECTOR_SIZE)) {
        return FLASH_UPDATE_TEST_PROGRAM_FAILED;
    }

    /* 4. 先确认原始数据确实写对了 */
    if (!w25q128_read(FLASH_TEST_ADDRESS,
                      test_actual,
                      W25Q128_SECTOR_SIZE)) {
        return FLASH_UPDATE_TEST_INITIAL_READ_FAILED;
    }

    for (uint32_t i = 0; i < W25Q128_SECTOR_SIZE; ++i) {
        if (test_actual[i] != test_expected[i]) {
            flash_test_bad_address = FLASH_TEST_ADDRESS + i;
            flash_test_expected_value = test_expected[i];
            flash_test_actual_value = test_actual[i];

            return FLASH_UPDATE_TEST_INITIAL_MISMATCH;
        }
    }

    /* 5. 只更新偏移 16、17 的两个字节 */
    if (!w25q128_update(FLASH_TEST_ADDRESS + 16U,
                        new_data,
                        sizeof(new_data))) {
        return FLASH_UPDATE_TEST_UPDATE_FAILED;
    }

    /* 修改期望结果，其余字节保持原值 */
    test_expected[16] = 0xFF;
    test_expected[17] = 0xFF;

    /* 6. 重新读回整个扇区 */
    if (!w25q128_read(FLASH_TEST_ADDRESS,
                      test_actual,
                      W25Q128_SECTOR_SIZE)) {
        return FLASH_UPDATE_TEST_FINAL_READ_FAILED;
    }

    /* 7. 比较全部 4096 字节 */
    for (uint32_t i = 0; i < W25Q128_SECTOR_SIZE; ++i) {
        if (test_actual[i] != test_expected[i]) {
            flash_test_bad_address = FLASH_TEST_ADDRESS + i;
            flash_test_expected_value = test_expected[i];
            flash_test_actual_value = test_actual[i];

            return FLASH_UPDATE_TEST_FINAL_MISMATCH;
        }
    }

    return FLASH_UPDATE_TEST_PASS;
}