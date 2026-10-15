/**
 * @file lv_port_lcd_stm32_template.c
 *
 * Example implementation of the LVGL LCD display drivers on the STM32 platform
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
/* Include STM32Cube files here, e.g.:
#include "stm32f7xx_hal.h"
*/

#include "lv_port_lcd_stm32.h"
#include "./src/drivers/display/st7789/lv_st7789.h"
#include "spi.h"
#include "lcd.h"
#include "lcd_init.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 320

#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    320
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif



/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t * lcd_disp;
static volatile int lcd_bus_busy = 0;

void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
   //创建显示设备
   lv_display_t * display =  lv_display_create(240,280);

   /* 👇 关键新增：告诉 LVGL 输出时做字节交换 */
   lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565_SWAPPED);

   static uint8_t buff[240*280 / 10 *2];
   lv_display_set_buffers(display,buff,NULL,sizeof(buff),LV_DISPLAY_RENDER_MODE_PARTIAL);
   lv_display_set_flush_cb(display,my_flush_cb);
}


/* Copy rendered image to screen.
 * This needs to be implemented by the user. */
void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    /* 1. 计算刷新区域的尺寸 */
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;
    uint32_t w = x2 - x1 + 1;
    uint32_t h = y2 - y1 + 1;
    uint32_t byte_len = w * h * 2;  // RGB565 每个像素占 2 字节

    /* 2. 直接调用你的 LCD 发送函数 */
    /*    注意：因为启用了 LV_COLOR_16_SWAP，color_p 指向的内存已经是 [高字节][低字节] 顺序 */
    /*    直接强转为 uint8_t* 发送即可，绝对不要再手动交换！ */
    LCD_MyFill(x1, y1, x2, y2, (uint8_t *)px_buf, byte_len);

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
