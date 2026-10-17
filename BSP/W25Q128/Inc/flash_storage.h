//
// Created by LightRain on 2026/10/17.
//

#ifndef F4_LVGL_DEMO2_FLASH_STORAGE_H
#define F4_LVGL_DEMO2_FLASH_STORAGE_H

#include "stdbool.h"
#include "stdint.h"

/*
 * @brief 向FREERTOS 申请一把锁
 * @note 仅调用一次;在启动FLASH的任务之前完成初始化
 */
bool flash_storage_init(void);

/*
 * @brief 给FLASH上锁
 */
static bool flash_block(void);

/*
 * @brief 解除FLASH的锁
 */
static void flash_unlock(void);

/*
 * @brief 线程安全的读flash
 */
bool flash_read(uint32_t address,uint8_t *data,uint32_t length);

/*
 * @brief 线程安全的写入，会自动分页
 */
bool flash_program(uint32_t address,const uint8_t *data,uint32_t length);

/*
 * @brief 线程安全的更新数据,任意地址写，并不会改变其他位置的数据
 */
bool flash_update(uint32_t address,const uint8_t *data,uint32_t length);

/*
 * @brief 线程安全的擦除FLASH,按扇区擦除
 */
bool flash_erase_sector(uint32_t address);

#endif // F4_LVGL_DEMO2_FLASH_STORAGE_H
