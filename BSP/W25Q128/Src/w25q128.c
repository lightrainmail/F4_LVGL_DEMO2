//
// Created by admin on 2026/10/16.
//
#include "w25q128.h"

/*
 * @brief w25q128 初始化函数
 * @note SPI CubeMX已经初始化，这里只用初始化CS PIN即可
 */
void w25q128_init(void) {
    flash_cs_pin_init();
}

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

/*
 * @brief 读取状态寄存器1
 */
bool w25q128_read_sr1(uint8_t *sr1) {
    uint8_t discard;
    bool ok = false;

    if (sr1 == NULL) {
        return false;
    }

    //拉低CS开始通讯
    flash_cs(false);

    //发送读取SR1命令
    ok = flash_spi_xfer(W25Q128_CMD_READ_SR1,&discard);

    if (ok) {
        //发送一个占位字节，同时读取SR1
        ok = flash_spi_xfer(0xFF,sr1);
    }

    //无论是否成功都释放片选
    flash_cs(true);

    return ok;
}

/*
 * @brief 带有超时的等待空闲函数
 * @param 空闲返回true;异常及超时返回false
 */
bool w25q128_wait_ready(uint32_t timeout) {
    uint8_t sr1;
    uint32_t start_tick;

    start_tick = HAL_GetTick();

    while (1) {
        if (!w25q128_read_sr1(&sr1)) {
            return false;
        }

        //BUSY 为0，代表FLASH已经空闲
        if ((sr1 & W25Q128_SR1_BUSY) == 0U) {
            return true;
        }

        if ((uint32_t)(HAL_GetTick() - start_tick) > timeout) {
            return false;
        }
    }
}

/*
 * @brief 实现写使能
 * @param 返回ture代表写使能设置成功;返回false代表出来各种乱七八糟的问题
 */
bool w25q128_write_enable(void) {
    uint8_t discard;
    uint8_t sr1;
    bool ok = false;

    //先等待空闲
    if (!w25q128_wait_ready(100U)) {
        return false;
    }

    //发送写使能命令
    flash_cs(false);

    ok = flash_spi_xfer(W25Q128_CMD_WRITE_ENABLE,&discard);

    flash_cs(true);

    //注意，读写不同的寄存器，CS要释放一次，以示一件事情完成

    if (!ok) {
        return false;
    }

    //重写读取SR1，检查WEL位
    if (!w25q128_read_sr1(&sr1)) {
        return false;
    }

    if ((sr1 & W25Q128_SR1_WEL) != 0U) {
        return true;
    }

    return false;
}

/*
 * @brief 取消写使能，禁止写入
 */
bool w25q128_write_disable(void) {
    uint8_t discard;
    uint8_t sr1;
    bool ok = false;

    //等待空闲
    if (!w25q128_wait_ready(100U)) {
        return false;
    }

    //发送写禁止命令
    flash_cs(false);

    ok = flash_spi_xfer(W25Q128_CMD_WRITE_DISABLE,&discard);

    flash_cs(true);

    if (!ok) {
        return false;
    }

    if (!w25q128_read_sr1(&sr1)) {
        return false;
    }

    if ((sr1 & W25Q128_SR1_WEL) == 0U){
        return true;
    }
    return false;
}

/*
 * @brief 普通数据读取
 */
bool w25q128_read(uint32_t address,uint8_t *buffer,uint32_t length) {
    uint8_t discard;
    bool ok;

    //检查地址范围
    if (address >= W25Q128_CAPACITY_BYTES) {
        return false;
    }

    //用减法检查范围，避免address + length 溢出，别读到芯片外面去了
    if (length > (W25Q128_CAPACITY_BYTES - address)) {
        return false;
    }

    if (length == 0U) {
        return false;
    }

    if (buffer == NULL) {
        return false;
    }

    //等待空闲
    if (!w25q128_wait_ready(100U)) {
        return false;
    }

    //拉低CS，开始通讯
    flash_cs(false);

    //先发送读数据指令
    ok = flash_spi_xfer(W25Q128_CMD_READ_DATA,&discard);

    //发送地址高8位
    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 16) & 0xFFU),&discard);
    }

    //发送中间8位
    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 8) & 0xFFU),&discard);
    }

    //发送最低8位
    if (ok) {
        ok = flash_spi_xfer((uint8_t)(address & 0xFFU),&discard);
    }

    //连续接收数据
    for (uint32_t i = 0; i < length && ok; i++) {
        ok = flash_spi_xfer(0xFF,&buffer[i]);
    }

    //释放CS，结束通讯
    flash_cs(true);

    return ok;
}

/*
 * @brief 擦除一个扇区，一个扇区4KiB
 * @param address,地址必须是扇区起始地址，4096U的整数倍，例如0x000000U 0x001000U 0x002000U 0xFFF000
 */
bool w25q128_erase_sector(uint32_t address) {
    uint8_t discard;
    bool ok;

    //检查地址范围
    if (address >= W25Q128_CAPACITY_BYTES) {
        return false;
    }

    //要求去4KiB对齐
    if ((address % W25Q128_SECTOR_SIZE) != 0U) {
        return false;
    }

    //写使能，这个函数内部已经有等待空闲的函数,并检查SR1的WEL位是否为1
    if (!w25q128_write_enable()) {
        return false;
    }

    //拉低CS，通讯开始
    flash_cs(false);

    //发送扇区擦除指令
    ok = flash_spi_xfer(W25Q128_CMD_SECTOR_ERASE,&discard);

    //发送地址，从高8位开始
    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 16) & 0xFFU),&discard);
    }

    //中间8位
    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 8) & 0xFFU),&discard);
    }

    //低8位
    if (ok) {
        ok = flash_spi_xfer((uint8_t)(address & 0xFFU),&discard);
    }

    //释放CS，结束通讯
    flash_cs(true);

    if (!ok) {
        return false;
    }

    //等待空闲，并设置3秒的等待时间
    return w25q128_wait_ready(5000U);
}

/*
 * @brief 页编程
 */
bool w25q128_page_program(uint32_t address,const uint8_t *data,uint32_t length) {
    uint8_t discard;
    bool ok;

    if (address >= W25Q128_CAPACITY_BYTES) {
        return false;
    }

    if (length == 0U || data == NULL) {
        return false;
    }

    /*检查是否超出跨越页边界 这一步计算address到页的末尾还有多少长度*/
    uint32_t page_remaining = W25Q128_PAGE_SIZE - (address % W25Q128_PAGE_SIZE);

    if (length > page_remaining) {
        return false;
    }

    if (!w25q128_write_enable()) {
        return false;
    }

    flash_cs(false);

    ok = flash_spi_xfer(W25Q128_CMD_PAGE_PROGRAM,&discard);

    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 16) & 0xFFU),&discard);
    }

    if (ok) {
        ok = flash_spi_xfer((uint8_t)((address >> 8) & 0xFFU),&discard);
    }

    if (ok) {
        ok = flash_spi_xfer((uint8_t)(address & 0xFFU),&discard);
    }

    for (uint32_t i = 0; i < length && ok; i++) {
        ok = flash_spi_xfer(data[i],&discard);
    }

    //释放CS，SPI通讯结束，W25Q128正式写入数据
    flash_cs(true);

    if (!ok) {
        return false;
    }

    return w25q128_wait_ready(100U);
}

/*
 * @brief 写入自动分页
 */
bool w25q128_program(uint32_t address,const uint8_t *data,uint32_t length) {
    uint32_t page_remaining;
    uint32_t write_length;

    if (address >= W25Q128_CAPACITY_BYTES) {
        return false;
    }

    if (length > (W25Q128_CAPACITY_BYTES - address)) {
        return false;
    }

    if (length == 0U) {
        return false;
    }

    if (data == NULL) {
        return false;
    }

    while (length > 0U) {
        /*当前页还能写多少字节*/
        page_remaining = W25Q128_PAGE_SIZE - (address % W25Q128_PAGE_SIZE);

        /* 默认写完剩余数据 */
        write_length = length;

        /* 当超过当前页容量时，只写到页末尾 */
        if (write_length > page_remaining) {
            write_length = page_remaining;
        }

        if (!w25q128_page_program(address,data,write_length)) {
            return false;
        }

        /* 移动到下一段 */
        address += write_length;    //Flash 地址向后移动
        data += write_length;       //数据指针向后移动
        length -= write_length;     //剩余长度减少
    }
    return true;
}

/*
 * @brief 任意地址更新，并保留同一扇区的其他数据
 */
static uint8_t sector_buffer[W25Q128_SECTOR_SIZE];

bool w25q128_update(uint32_t address,const uint8_t *data,uint32_t length) {
    uint32_t sector_address;
    uint32_t sector_offset;
    uint32_t update_length;

    if (address >= W25Q128_CAPACITY_BYTES) {
        return false;
    }

    if (length > (W25Q128_CAPACITY_BYTES - address)) {
        return false;
    }

    if (length == 0U) {
        return false;
    }

    if (data == NULL) {
        return false;
    }

    while (length > 0U) {
        /* 计算当前地址在扇区内的偏移 */
        sector_offset = address % W25Q128_SECTOR_SIZE;

        /* 计算当前扇区的起始地址 */
        sector_address = address - sector_offset;

        /* 计算当前扇区还能容纳的数据长度 */
        update_length = W25Q128_SECTOR_SIZE - sector_offset;

        if (update_length > length) {
            update_length = length;
        }

        /*将整个扇区的数据读到RAM中*/
        if (!w25q128_read(sector_address,sector_buffer,W25Q128_SECTOR_SIZE)) {
            return false;
        }

        /* 新旧数据比较，相同就跳过 memcpy(*data1,*data2,length)用于比较两组数据是否不相同，相同返回0;不相同返回1 */
        if (memcmp(&sector_buffer[sector_offset],data,update_length) != 0) {
            /* 只更改缓存中的目标区域 */
            memcpy(&sector_buffer[sector_offset],data,update_length);

            //擦除整个扇区
            if (!w25q128_erase_sector(sector_address)) {
                return false;
            }

            /*自动分页写回整个扇区*/
            if (!w25q128_program(sector_address,sector_buffer,W25Q128_SECTOR_SIZE)) {
                return false;
            }
        }

        address += update_length;
        data += update_length;
        length -= update_length;
    }

    return true;
}









