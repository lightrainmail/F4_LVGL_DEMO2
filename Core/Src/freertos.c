/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "lcd_init.h"
#include "lv_port_indev.h"
#include "lv_port_lcd_stm32.h"
#include "lvgl.h"
#include "ui.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 按钮点击回调 */
static void btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t *btn = lv_event_get_target(e);
        lv_obj_t *label = lv_obj_get_child(btn, 0); // 获取按钮上的标签
        lv_label_set_text(label, "Clicked!"); // 改变文字
    }
}

/* 创建底部按钮，在 main 中调用一次 */
void lv_demo_bottom_button(void) {
    lv_obj_t *scr = lv_scr_act(); // 获取当前屏幕

    lv_obj_t *btn = lv_btn_create(scr); // 创建按钮
    lv_obj_set_size(btn, 150, 50); // 设置宽高
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20); // 底部居中，离底边20像素

    lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), 0);

    lv_obj_t *label = lv_label_create(btn); // 按钮上的文字
    lv_label_set_text(label, "Press Me");
    lv_obj_center(label); // 文字居中

    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
}
/* USER CODE END Variables */
/* Definitions for main */
osThreadId_t mainHandle;
const osThreadAttr_t main_attributes = {
  .name = "main",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for lvgl */
osThreadId_t lvglHandle;
const osThreadAttr_t lvgl_attributes = {
  .name = "lvgl",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_buf);
/* USER CODE END FunctionPrototypes */

void App_main(void *argument);
void App_lvgl(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of main */
  mainHandle = osThreadNew(App_main, NULL, &main_attributes);

  /* creation of lvgl */
  lvglHandle = osThreadNew(App_lvgl, NULL, &lvgl_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_App_main */
/**
 * @brief  Function implementing the main thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_App_main */
void App_main(void *argument)
{
  /* USER CODE BEGIN App_main */

    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
  /* USER CODE END App_main */
}

/* USER CODE BEGIN Header_App_lvgl */
/**
 * @brief Function implementing the lvgl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_App_lvgl */
void App_lvgl(void *argument)
{
  /* USER CODE BEGIN App_lvgl */
    LCD_Init();

    // 1.调用lv_init
    lv_init();
    // 2.设置时钟
    lv_tick_set_cb(HAL_GetTick);
    lv_delay_set_cb(vTaskDelay);

    // 3.初始化显示设备
    lv_port_disp_init();

    // 4.初始化触摸设备
    lv_port_indev_init();

    /* Create widgets */
    create_button();
    /* 等价写法：lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0); */

    /* Infinite loop */
    for (;;) {
        uint32_t tick = lv_timer_handler();
        vTaskDelay(tick);
    }
  /* USER CODE END App_lvgl */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

