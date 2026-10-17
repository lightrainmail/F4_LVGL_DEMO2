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


/* 创建测试界面：初始化完成后调用一次 */
void ui_button_test_create(void);

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
uint8_t flash_storage_test(void);

#endif // F4_LVGL_DEMO2_UI_H
