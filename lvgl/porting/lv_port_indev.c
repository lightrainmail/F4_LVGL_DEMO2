/**
 * @file lv_port_indev_template.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#include "CST816T.h"
#include "i2c.h"        /* 提供 hi2c1 的声明 */

/*********************
 *      DEFINES
 *********************/
#define SCREEN_W        240
#define SCREEN_H        280

/* 触摸 IC 原始坐标最大值，需实测 V8 里用的是 222，先沿用 */
#define TOUCH_RAW_X_MAX 222
#define TOUCH_RAW_Y_MAX 280

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t * indev_touchpad;

static cst816d_handle_t cstHandle;
static cst816d_info_t   cstInfo;

static uint16_t screen_x = 0;
static uint16_t screen_y = 0;

static void touchpad_init(void);
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);

static lv_indev_t *     indev_touchpad;
static cst816d_handle_t cstHandle;
static cst816d_info_t   cstInfo;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/*
 * @brief 添加触摸设备
 */
void lv_port_indev_init(void)
{
    /* 1. 初始化触摸 IC（硬件 IIC） */
    touchpad_init();

    /* 2. 创建 LVGL 输入设备对象（V9 新 API） */
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    /* 单屏情况下无需显式绑定 display，默认绑定最后创建的显示设备 */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void touchpad_init(void)
{
    /* 硬件 IIC：传入 &hi2c1；RST/INT 引脚按你实际接线填 */
    uint8_t ret = cst816t_init(&cstHandle, &hi2c1,
                               GPIOB, GPIO_PIN_8,   /* RST */
                               GPIOB, GPIO_PIN_9);  /* INT */
    (void)ret;  /* 想调试就 printf 一下 ret */
}

/* 回调签名：V8 的 lv_indev_drv_t* → V9 的 lv_indev_t* */
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    if (cst816t_touch_value(&cstHandle, &cstInfo) == 0) {
        /* 用 finger_num > 0 判断按下，比 event==2 更稳，
         * 快速点击时不会漏掉第一帧 */
        if (cstInfo.finger_num > 0) {
            int32_t x = (int32_t)cstInfo.point.x * SCREEN_W / TOUCH_RAW_X_MAX;
            int32_t y = (int32_t)cstInfo.point.y * SCREEN_H / TOUCH_RAW_Y_MAX;

            if (x < 0)         x = 0;
            if (x >= SCREEN_W) x = SCREEN_W - 1;
            if (y < 0)         y = 0;
            if (y >= SCREEN_H) y = SCREEN_H - 1;

            last_x = x;
            last_y = y;
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    } else {
        /* IIC 读取失败，按未按下处理 */
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /* 抬起时也要把最后坐标填回去，否则拖拽/长按判定会出问题 */
    data->point.x = last_x;
    data->point.y = last_y;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
