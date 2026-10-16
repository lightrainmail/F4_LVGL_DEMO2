//
// Created by admin on 2026/10/16.
//
#include "w25q128_port.h"



/*
 * @brief flash CS 引脚接口函数
 * @param high:false 拉低CS，选中FLASH;high:true 拉高CS，不选中FLASH
 */
void flash_cs(bool high) {
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, high?GPIO_PIN_SET:GPIO_PIN_RESET);
}

/*
 * @brief SPI传输接口函数
 * @param tx:8bit Data;rx 接受缓冲区指针
 */
bool flash_spi_xfer(uint8_t tx,uint8_t *rx) {
    if (rx == NULL) {
        return false;
    }

    return HAL_SPI_TransmitReceive(&hspi2,&tx,rx,1,100) == HAL_OK;
}

/*
 * @brief FLAHS CS Pin init
 */
void flash_cs_pin_init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.Pin = FLASH_CS_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLASH_CS_Port, &GPIO_InitStructure);

    //初始化后保持CS脚为高电平，不选中FLASH
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}
