//
// Created by admin on 2026/10/16.
//

#ifndef F4_LVGL_DEMO2_W25Q128_H
#define F4_LVGL_DEMO2_W25Q128_H

#include "w25q128_port.h"
#include "w25q128_defs.h"

/*
 * @brief 读取器件ID
 */
bool w25q128_read_jedec_id(uint8_t id[3]);


#endif //F4_LVGL_DEMO2_W25Q128_H
