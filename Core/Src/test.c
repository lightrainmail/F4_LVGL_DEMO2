//
// Created by LightRain on 2026/10/17.
//
#include "test.h"

/* 4096 字节扇区会使这个对象较大，放在静态存储区 */
static FATFS flash_fs;

volatile FRESULT flash_mount_result;

/* 工作缓冲区至少容纳一个逻辑扇区，放在静态区 */
static BYTE format_work[4096];

volatile FRESULT flash_format_result;
volatile FRESULT flash_remount_result;
volatile uint8_t flash_format_stage = 0;

/*
 * @brief FLASH 挂在测试
 */
void flash_mount_test(void)
{
    flash_mount_result = f_mount(&flash_fs, USERPath, 1);
}

void flash_format_test(void)
{
    flash_format_stage = 1;

    /* 注销之前的挂载对象，不会取消底层驱动注册 */
    flash_format_result = f_mount(NULL, USERPath, 0);

    if (flash_format_result != FR_OK) {
        return;
    }

    flash_format_stage = 2;

    /* FatFs R0.12c 的五参数接口 */
    flash_format_result = f_mkfs(
        USERPath,
        FM_FAT | FM_SFD,
        0,
        format_work,
        sizeof(format_work));

    if (flash_format_result != FR_OK) {
        return;
    }

    flash_format_stage = 3;

    /* 格式化成功后，重新挂载 */
    flash_remount_result = f_mount(&flash_fs, USERPath, 1);

    if (flash_remount_result != FR_OK) {
        return;
    }

    flash_format_stage = 4;
}

/* FIL 可能包含扇区缓存，放在静态区 */
static FIL test_file;

volatile FRESULT file_test_result;
volatile uint8_t file_test_stage = 0;
volatile bool file_test_pass = false;

void flash_file_test(void)
{
    const char text[] = "Hello STM32 FatFs!\r\n";
    char read_buffer[64] = {0};
    char path[32];

    UINT written = 0;
    UINT read_count = 0;
    FRESULT close_result;

    file_test_pass = false;
    file_test_stage = 1;

    /* 根据实际盘符构造文件路径 */
    snprintf(path, sizeof(path), "%s/TEST.TXT", USERPath);

    /* 1. 挂载 */
    file_test_result = f_mount(&flash_fs, USERPath, 1);

    if (file_test_result != FR_OK) {
        return;
    }

    /* 2. 创建文件，已存在则覆盖 */
    file_test_stage = 2;

    file_test_result = f_open(
        &test_file,
        path,
        FA_CREATE_ALWAYS | FA_WRITE);

    if (file_test_result != FR_OK) {
        return;
    }

    /* 3. 写入，不写入字符串末尾的 '\0' */
    file_test_stage = 3;

    file_test_result = f_write(
        &test_file,
        text,
        sizeof(text) - 1U,
        &written);

    /* 即使写入失败，也要尝试关闭文件 */
    close_result = f_close(&test_file);

    if (file_test_result != FR_OK) {
        return;
    }

    if (close_result != FR_OK) {
        file_test_stage = 4;
        file_test_result = close_result;
        return;
    }

    if (written != sizeof(text) - 1U) {
        return;
    }

    /* 4. 重新以只读方式打开 */
    file_test_stage = 5;

    file_test_result = f_open(&test_file, path, FA_READ);

    if (file_test_result != FR_OK) {
        return;
    }

    /* 5. 读取 */
    file_test_stage = 6;

    file_test_result = f_read(
        &test_file,
        read_buffer,
        sizeof(read_buffer) - 1U,
        &read_count);

    close_result = f_close(&test_file);

    if (file_test_result != FR_OK) {
        return;
    }

    if (close_result != FR_OK) {
        file_test_stage = 7;
        file_test_result = close_result;
        return;
    }

    /* 6. 比较长度和内容 */
    file_test_stage = 8;

    if (read_count != sizeof(text) - 1U) {
        return;
    }

    if (memcmp(read_buffer, text, sizeof(text) - 1U) != 0) {
        return;
    }

    file_test_pass = true;
    file_test_stage = 9;
}

void flash_file_read_only_test(void)
{
    const char expected[] = "Hello STM32 FatFs!\r\n";

    char read_buffer[64] = {0};
    char path[32];

    UINT read_count = 0;
    FRESULT close_result;

    file_test_pass = false;
    file_test_stage = 1;

    snprintf(path, sizeof(path), "%s/TEST.TXT", USERPath);

    /* 1. 挂载已有文件系统 */
    file_test_result = f_mount(&flash_fs, USERPath, 1);

    if (file_test_result != FR_OK) {
        return;
    }

    /* 2. 只读打开，不创建、不覆盖 */
    file_test_stage = 2;

    file_test_result = f_open(&test_file, path, FA_READ);

    if (file_test_result != FR_OK) {
        return;
    }

    /* 3. 读取已有文件 */
    file_test_stage = 3;

    file_test_result = f_read(
        &test_file,
        read_buffer,
        sizeof(read_buffer) - 1U,
        &read_count);

    close_result = f_close(&test_file);

    if (file_test_result != FR_OK) {
        return;
    }

    if (close_result != FR_OK) {
        file_test_stage = 4;
        file_test_result = close_result;
        return;
    }

    /* 4. 检查内容 */
    file_test_stage = 5;

    if (read_count != sizeof(expected) - 1U) {
        return;
    }

    if (memcmp(read_buffer, expected, sizeof(expected) - 1U) != 0) {
        return;
    }

    file_test_pass = true;
    file_test_stage = 6;
}