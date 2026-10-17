//
// Created by admin on 2026/10/16.
//

#ifndef F4_LVGL_DEMO2_W25Q128_INTERFACE_H
#define F4_LVGL_DEMO2_W25Q128_INTERFACE_H

#include "stm32f4xx.h"
#include "stdio.h"
#include "stdbool.h"
#include "spi.h"
#include "w25q128_defs.h"

//CS引脚定义
#define FLASH_CS_Port   GPIOB
#define FLASH_CS_Pin    GPIO_PIN_12

/*
 * @brief flash CS 引脚接口函数
 * @param high:false 拉低CS，选中FLASH;high:true 拉高CS，不选中FLASH
 */
void flash_cs(bool high);

/*
 * @brief SPI传输接口函数
 * @param tx:8bit Data;rx 接受缓冲区指针
 */
bool flash_spi_xfer(uint8_t tx,uint8_t *rx);

/*
 * @brief FLAHS CS Pin init
 */
void flash_cs_pin_init(void);

#endif //F4_LVGL_DEMO2_W25Q128_INTERFACE_H
