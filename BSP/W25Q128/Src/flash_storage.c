//
// Created by LightRain on 2026/10/17.
//
#include "flash_storage.h"
#include "w25q128.h"

#include "FreeRTOS.h"
#include "semphr.h"

static SemaphoreHandle_t flash_mutex = NULL;

/*
 * @brief 向FREERTOS 申请一把锁
 * @note 仅调用一次;在启动FLASH的任务之前完成初始化
 */
bool flash_storage_init(void) {
    if (flash_mutex != NULL) {
        return true;
    }

    flash_mutex = xSemaphoreCreateMutex();

    if (flash_mutex == NULL) {
        return false;
    }
}

/*
 * @brief 给FLASH上锁
 */
static bool flash_block(void) {
    if (flash_mutex == NULL) {
        return false;
    }

    if (xSemaphoreTake(flash_mutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }

    return true;
}

/*
 * @brief 解除FLASH的锁
 */
static void flash_unlock(void) {
    xSemaphoreGive(flash_mutex);
}

/*
 * @brief 线程安全的读flash
 */
bool flash_read(uint32_t address,uint8_t *data,uint32_t length) {
    bool ok;

    //flash上锁
    if (!flash_block()) {
        return false;
    }

    ok = w25q128_read(address,data,length);

    //解锁
    flash_unlock();

    return ok;
}

/*
 * @brief 线程安全的写入，会自动分页
 */
bool flash_program(uint32_t address,const uint8_t *data,uint32_t length) {
    bool ok;

    if (!flash_block()) {
        return false;
    }

    ok = w25q128_program(address,data,length);

    flash_unlock();

    return ok;
}

/*
 * @brief 线程安全的更新数据,任意地址写，并不会改变其他位置的数据
 */
bool flash_update(uint32_t address,const uint8_t *data,uint32_t length) {
    bool ok;

    if (!flash_block()) {
        return false;
    }

    ok = w25q128_update(address,data,length);

    flash_unlock();

    return ok;
}

/*
 * @brief 线程安全的擦除FLASH,按扇区擦除
 */
bool flash_erase_sector(uint32_t address) {
    bool ok;

    if (!flash_block()) {
        return false;
    }

    ok = w25q128_erase_sector(address);

    flash_unlock();

    return ok;
}
















