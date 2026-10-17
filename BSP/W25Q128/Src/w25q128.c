//
// Created by admin on 2026/10/16.
//
#include "w25q128.h"

/*
 * @brief 读取器件ID
 */
bool w25q128_read_jedec_id(uint8_t id[3]) {
    uint8_t discard;
    bool ok = false;

    if (id == NULL) {
        return false;
    }

    //拉低CS
    flash_cs(false);

    ok = flash_spi_xfer(W25Q128_CMD_JEDEC_ID,&discard);

    //前一次接收成功，才会进行下次接收
    for(uint8_t i  = 0; i < 3 && ok;++i) {
        ok = flash_spi_xfer(0xFF,&id[i]);
    }

    flash_cs(true);

    return ok;
}