//
// Created by LightRain on 2026/10/17.
//

#ifndef F4_LVGL_DEMO2_TEST_H
#define F4_LVGL_DEMO2_TEST_H

#include "fatfs.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

/*
 * @brief FLASH 挂在测试
 */
void flash_mount_test(void);

void flash_format_test(void);

void flash_file_test(void);

void flash_file_read_only_test(void);

#endif // F4_LVGL_DEMO2_TEST_H
