#ifndef W25Q128_DEFS_H
#define W25Q128_DEFS_H

/* ==================== 存储参数 ==================== */

#define W25Q128_CAPACITY_BYTES           0x01000000UL
#define W25Q128_PAGE_SIZE                256UL
#define W25Q128_SECTOR_SIZE              4096UL
#define W25Q128_BLOCK32_SIZE             32768UL
#define W25Q128_BLOCK64_SIZE             65536UL

/* ==================== 写使能 ==================== */

#define W25Q128_CMD_WRITE_ENABLE         0x06U  /* 写使能 */
#define W25Q128_CMD_WRITE_DISABLE        0x04U  /* 写禁止 */
#define W25Q128_CMD_VOLATILE_SR_WREN      0x50U  /* 易失状态寄存器写使能 */

/* ==================== 状态寄存器 ==================== */

#define W25Q128_CMD_READ_SR1             0x05U
#define W25Q128_CMD_READ_SR2             0x35U
#define W25Q128_CMD_READ_SR3             0x15U

#define W25Q128_CMD_WRITE_SR1            0x01U
#define W25Q128_CMD_WRITE_SR2            0x31U
#define W25Q128_CMD_WRITE_SR3            0x11U

/* ==================== 数据读取 ==================== */

#define W25Q128_CMD_READ_DATA            0x03U  /* 普通读取 */
#define W25Q128_CMD_FAST_READ            0x0BU  /* 快速读取 */

/* ==================== 编程与擦除 ==================== */

#define W25Q128_CMD_PAGE_PROGRAM         0x02U  /* 页编程 */
#define W25Q128_CMD_SECTOR_ERASE         0x20U  /* 4 KiB 擦除 */
#define W25Q128_CMD_BLOCK32_ERASE        0x52U  /* 32 KiB 擦除 */
#define W25Q128_CMD_BLOCK64_ERASE        0xD8U  /* 64 KiB 擦除 */
#define W25Q128_CMD_CHIP_ERASE           0xC7U  /* 整片擦除 */
#define W25Q128_CMD_CHIP_ERASE_ALT       0x60U  /* 整片擦除的另一命令码 */

/* ==================== 暂停与恢复 ==================== */

#define W25Q128_CMD_SUSPEND              0x75U  /* 暂停擦除或编程 */
#define W25Q128_CMD_RESUME               0x7AU  /* 恢复擦除或编程 */

/* ==================== 器件信息 ==================== */

#define W25Q128_CMD_JEDEC_ID             0x9FU  /* JEDEC ID */
#define W25Q128_CMD_MANUFACTURER_ID      0x90U  /* 厂商及器件 ID */
#define W25Q128_CMD_UNIQUE_ID            0x4BU  /* 唯一序列号 */
#define W25Q128_CMD_READ_SFDP            0x5AU  /* SFDP 参数 */

/* ==================== 掉电与复位 ==================== */

#define W25Q128_CMD_POWER_DOWN           0xB9U  /* 深度掉电 */
#define W25Q128_CMD_RELEASE_POWER_DOWN   0xABU  /* 退出掉电，也可读器件 ID */
#define W25Q128_CMD_ENABLE_RESET         0x66U  /* 复位使能 */
#define W25Q128_CMD_RESET_DEVICE         0x99U  /* 执行复位 */

/* ==================== 安全寄存器 ==================== */

#define W25Q128_CMD_READ_SECURITY        0x48U
#define W25Q128_CMD_PROGRAM_SECURITY     0x42U
#define W25Q128_CMD_ERASE_SECURITY       0x44U

/* ==================== 区域锁定 ==================== */

#define W25Q128_CMD_BLOCK_LOCK          0x36U
#define W25Q128_CMD_BLOCK_UNLOCK        0x39U
#define W25Q128_CMD_READ_BLOCK_LOCK      0x3DU
#define W25Q128_CMD_GLOBAL_LOCK          0x7EU
#define W25Q128_CMD_GLOBAL_UNLOCK        0x98U

/* ==================== Dual / Quad SPI ==================== */
/* 需要对应的多线传输时序，当前普通 SPI 驱动不使用 */

#define W25Q128_CMD_FAST_READ_DUAL_OUT   0x3BU
#define W25Q128_CMD_FAST_READ_DUAL_IO    0xBBU
#define W25Q128_CMD_ID_DUAL_IO           0x92U

#define W25Q128_CMD_FAST_READ_QUAD_OUT   0x6BU
#define W25Q128_CMD_FAST_READ_QUAD_IO    0xEBU
#define W25Q128_CMD_ID_QUAD_IO           0x94U
#define W25Q128_CMD_QUAD_PAGE_PROGRAM    0x32U
#define W25Q128_CMD_SET_BURST_WRAP       0x77U

/* ==================== SR1 位掩码 ==================== */

#define W25Q128_SR1_BUSY                 0x01U  /* bit0 */
#define W25Q128_SR1_WEL                  0x02U  /* bit1 */
#define W25Q128_SR1_BP0                  0x04U  /* bit2 */
#define W25Q128_SR1_BP1                  0x08U  /* bit3 */
#define W25Q128_SR1_BP2                  0x10U  /* bit4 */
#define W25Q128_SR1_TB                   0x20U  /* bit5 */
#define W25Q128_SR1_SEC                  0x40U  /* bit6 */
#define W25Q128_SR1_SRP                  0x80U  /* bit7 */

#endif