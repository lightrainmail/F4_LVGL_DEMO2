//
// Created by admin on 2026/10/16.
//

#ifndef F4_LVGL_DEMO2_W25Q128_H
#define F4_LVGL_DEMO2_W25Q128_H

#include "w25q128_port.h"
#include "w25q128_defs.h"

/*
 * @brief w25q128 初始化函数
 * @note SPI CubeMX已经初始化，这里只用初始化CS PIN即可
 */
void w25q128_init(void);

/*
 * @brief 读取器件ID
 */
bool w25q128_read_jedec_id(uint8_t id[3]);

/*
 * @brief 读取状态寄存器1
 */
bool w25q128_read_sr1(uint8_t *sr1);

/*
 * @brief 带有超时的等待空闲函数
 */
bool w25q1238_wait_ready(uint32_t timeout);

/*
 * @brief 实现写使能
 * @param 返回ture代表写使能设置成功;返回false代表出来各种乱七八糟的问题
 */
bool w25q128_write_enable(void);

/*
 * @brief 取消写使能，禁止写入
 */
bool w25q128_write_disable(void);

/*
 * @brief 普通数据读取
 */
bool w25q128_read(uint32_t address,uint8_t *buffer,uint32_t length);

/*
 * @brief 擦除一个扇区，一个扇区4KiB
 * @param address,地址必须是扇区起始地址，4096U的整数倍，例如0x000000U 0x001000U 0x002000U 0xFFF000
 */
bool w25q128_erase_sector(uint32_t address);

/*
 * @brief 页编程
 */
bool w25q128_page_program(uint32_t address,const uint8_t *data,uint32_t length);

/*
 * @brief 写入自动分页
 */
bool w25q128_program(uint32_t address,const uint8_t *data,uint32_t length);


#endif //F4_LVGL_DEMO2_W25Q128_H
