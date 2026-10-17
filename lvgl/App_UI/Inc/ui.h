//
// Created by LightRain on 2026/10/13.
//

#ifndef F4_LVGL_DEMO2_UI_H
#define F4_LVGL_DEMO2_UI_H

#include "lvgl.h"
#include "w25q128.h"
#include "w25q128_defs.h"

#include "w25q128.h"
#include "w25q128_defs.h"

#define FLASH_TEST_ADDRESS  0xFFF000UL

typedef enum {
    FLASH_UPDATE_TEST_PASS = 0,
    FLASH_UPDATE_TEST_ERASE_FAILED,
    FLASH_UPDATE_TEST_PROGRAM_FAILED,
    FLASH_UPDATE_TEST_INITIAL_READ_FAILED,
    FLASH_UPDATE_TEST_INITIAL_MISMATCH,
    FLASH_UPDATE_TEST_UPDATE_FAILED,
    FLASH_UPDATE_TEST_FINAL_READ_FAILED,
    FLASH_UPDATE_TEST_FINAL_MISMATCH
} flash_update_test_result_t;

/* 使用静态数组，避免占用 FreeRTOS 任务栈 */
static uint8_t test_expected[W25Q128_SECTOR_SIZE];
static uint8_t test_actual[W25Q128_SECTOR_SIZE];



flash_update_test_result_t flash_update_test(void);

/* 创建测试界面：初始化完成后调用一次 */
void ui_button_test_create(void);

#endif // F4_LVGL_DEMO2_UI_H
