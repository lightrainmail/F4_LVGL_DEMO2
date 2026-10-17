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
#include "w25q128.h"
#include "w25q128_port.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define FLASH_TEST_ADDRESS  0xFFF000UL
flash_update_test_result_t result;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

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
  .stack_size = 1024 * 4,
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
    w25q128_init();

    result = flash_update_test();

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

