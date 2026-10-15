# F4_LVGL_DEMO2

基于 **STM32F411RET6 + FreeRTOS + LVGL 9** 的触摸屏学习工程，使用 STM32 HAL，通过 SPI1 驱动 ST7789 液晶屏，通过 I²C1 读取 CST816T 触摸控制器。

本文依据当前目录中的源码、CubeMX 配置和 CMake 脚本整理。编译命令供复现使用，本文整理过程中未重新编译、烧录或进行实机验证。

## 1. 当前功能

- 初始化 240 × 280 竖屏，以 RGB565 格式显示 LVGL 界面。
- 在屏幕中央创建一个 120 × 50 的按钮，文字为 `Click me`。
- 轮询触摸坐标，将按下、释放状态传递给 LVGL。
- 按钮点击后执行 `LV_LOG_USER("Clicked")`。当前 `LV_USE_LOG=0`，因此默认不会输出该日志，也不会改变按钮文字。
- 配置已启用右下角性能监视器和左下角 LVGL 内存监视器。

`Core/Src/freertos.c` 中还保留了 `lv_demo_bottom_button()`，其按钮文字可由 `Press Me` 变成 `Clicked!`，但启动流程没有调用该函数。

## 2. 软件与硬件配置

| 项目 | 当前配置 / 依据 |
| --- | --- |
| MCU | STM32F411RET6，Cortex-M4，硬件浮点 |
| 存储空间 | Flash 512 KiB，RAM 128 KiB，见链接脚本 |
| 外部时钟 | HSE 25 MHz |
| 系统时钟 | 100 MHz；PLL M=12、N=96、P=2 |
| 总线时钟 | AHB 100 MHz，APB1 50 MHz，APB2 100 MHz |
| LCD | ST7789，当前可见区域 240 × 280，默认竖屏 |
| SPI1 | 主机、8 bit、MSB first、Mode 0、软件 NSS、2 分频；按当前时钟计算为 50 MHz |
| 触摸 | CST816T 驱动，I²C 7 位地址 `0x15`，初始化接受芯片 ID `0xB5` / `0xB6` |
| I²C1 | 100 kHz，硬件 I²C |
| USART1 | 115200 baud，8N1，无硬件流控 |
| HAL 时基 | TIM11 |
| RTOS | FreeRTOS V10.3.1，CMSIS-RTOS V2 接口，1 kHz tick |
| LVGL | 本地 `lv_version.h` 标识为 9.5.0 |
| CubeMX | `.ioc` 记录版本 6.18.0 |
| STM32 固件包 | `.ioc` 记录 STM32Cube FW_F4 V1.28.3 |
| 构建系统 | CMake ≥ 3.22、Ninja、GNU Arm Embedded 工具链，C11 |

以上参数描述当前工程配置；更换开发板、晶振或屏幕时应同步调整配置与驱动。

## 3. 引脚连接

### LCD

引脚定义位于 `BSP/LCD/Inc/lcd_init.h`，SPI 复用配置位于 `Core/Src/spi.c`。

| 屏幕信号 | MCU 引脚 | 作用 |
| --- | --- | --- |
| SCL / SCLK | PB3 | SPI1 时钟 |
| SDA / MOSI | PB5 | SPI1 数据输出；这里的 SDA 是 SPI 数据线 |
| RES / RST | PB4 | LCD 硬件复位 |
| DC | PC12 | 命令 / 数据选择 |
| CS | PD2 | 软件片选 |
| BLK | PA15 | 背光控制，LCD 初始化时置高 |

### 触摸与串口

| 信号 | MCU 引脚 | 作用 |
| --- | --- | --- |
| Touch SCL | PB6 | I²C1 时钟 |
| Touch SDA | PB7 | I²C1 数据 |
| Touch RST | PB8 | 触摸驱动传入的复位脚 |
| Touch INT | PB9 | 配置为下降沿 EXTI，当前 LVGL 输入通过轮询读取 |
| USART1 TX | PA9 | 串口发送 |
| USART1 RX | PA10 | 串口接收 |

电源、电平和背光供电应以实际屏幕模组规格为准，所有设备需要共地。PB3、PB4、PA15 与调试复用功能有关，连接调试器时应核对 SWD / JTAG 配置。

**触摸复位配置待核对：** 当前输入移植层使用 PB8 进行复位，但 `MX_GPIO_Init()` 未将 PB8 配置为输出，触摸驱动也未初始化该 GPIO。若复位无效，应先补齐 GPIO 初始化并核对接线。

## 4. 目录结构

```text
F4_LVGL_DEMO2/
├── BSP/
│   ├── LCD/                      # LCD 初始化、绘图、字库与图片数据
│   └── TOUCH/                    # CST816T 触摸驱动
├── Core/
│   ├── Inc/                      # 外设头文件、HAL 与 FreeRTOS 配置
│   └── Src/                      # 启动流程、外设初始化、RTOS 任务、中断
├── Drivers/                      # CMSIS 与 STM32F4 HAL
├── Middlewares/Third_Party/FreeRTOS/
├── lvgl/
│   ├── src/                      # LVGL 库源码
│   ├── porting/                  # LCD 刷新与触摸输入适配
│   ├── App_UI/                    # 应用界面
│   ├── lv_conf.h                 # LVGL 功能与资源配置
│   └── lv_version.h              # LVGL 版本
├── cmake/
│   ├── gcc-arm-none-eabi.cmake    # 默认 GCC 交叉编译配置
│   ├── starm-clang.cmake          # 另附的 Clang 工具链配置
│   └── stm32cubemx/               # CubeMX 源文件、依赖与编译宏
├── build/                        # CMake 预设构建输出
├── cmake-build-debug-stm32/       # 现有 IDE 构建目录
├── CMakeLists.txt                # 顶层构建入口
├── CMakePresets.json             # Debug / Release 预设
├── F4_LVGL_DEMO2.ioc             # CubeMX 工程配置
├── startup_stm32f411xe.s          # 启动汇编
└── STM32F411xx_FLASH.ld           # Flash / RAM 布局
```

`BSP/LCD/LCD_Readme.md` 当前为空，LCD 使用说明集中在本文与驱动源码中。

## 5. 启动与任务流程

```text
main()
  ├─ HAL_Init() → SystemClock_Config()
  ├─ GPIO / DMA / I²C1 / SPI1 / USART1 初始化
  ├─ osKernelInitialize() → MX_FREERTOS_Init()
  └─ osKernelStart()
       ├─ App_main：循环 osDelay(1)
       └─ App_lvgl
            ├─ LCD_Init()
            ├─ lv_init()
            ├─ 注册 HAL_GetTick 和 vTaskDelay 回调
            ├─ lv_port_disp_init()
            ├─ lv_port_indev_init()
            ├─ create_button()
            └─ 循环 lv_timer_handler() → vTaskDelay(tick)
```

| 任务 | 优先级 | 配置栈空间 | 作用 |
| --- | --- | --- | --- |
| `main` | Normal | 512 字节 | 当前仅延时循环 |
| `lvgl` | Low | 4096 字节 | 显示、输入初始化与 LVGL 定时处理 |

FreeRTOS 使用 `heap_4.c`，`configTOTAL_HEAP_SIZE` 为 15360 字节。LVGL 使用独立的内置内存池，`LV_MEM_SIZE` 为 32 KiB；两者不是同一内存池。任务栈由 RTOS 堆分配，不应在估算 RAM 时重复累计。

### 显示路径

`lv_port_disp_init()` 实际调用 `lv_display_create(240, 280)`，设置 `LV_COLOR_FORMAT_RGB565_SWAPPED`，使用单缓冲、局部刷新模式。

- 缓冲大小为 `240 × 280 ÷ 10 × 2 = 13440` 字节。
- 刷新回调为 `my_flush_cb()`，调用 `LCD_MyFill()` 发送像素。
- 竖屏像素刷新在 `lcd.c` 中增加 Y 方向 20 像素偏移。
- 当前使用阻塞式 `HAL_SPI_Transmit()`，发送返回后调用 `lv_display_flush_ready()`。

虽然工程已配置 SPI1 TX DMA，且部分注释写有“DMA方式”，当前像素发送路径并未使用 DMA。`LV_USE_ST7789=1` 也不代表正在使用 LVGL 内置 ST7789 创建设备接口；当前显示仍由自定义刷新回调和 BSP 驱动完成。

### 触摸路径

`lv_port_indev.c` 注册指针类型输入设备，通过 `cst816t_touch_value()` 轮询获取触点；以 `finger_num > 0` 判断按下，读取失败时按释放处理，释放时保留最后坐标。

当前坐标换算为 `x = raw_x × 240 / 222`、`y = raw_y × 280 / 280`，并限制到屏幕范围。`TOUCH_RAW_X_MAX=222` 是沿用的标定参数，更换模组时需要实测校准。

## 6. 编译与烧录

在 `PATH` 中准备 `cmake`、`ninja` 和 `arm-none-eabi-gcc` 等工具。工程已包含 HAL、FreeRTOS 与 LVGL 源码，默认预设使用 GCC 工具链。

在项目根目录打开 PowerShell：

```powershell
# Debug
cmake --preset Debug
cmake --build --preset Debug

# Release
cmake --preset Release
cmake --build --preset Release
```

Debug 输出目录为 `build/Debug/`，Release 为 `build/Release/`。主要产物为 `F4_LVGL_DEMO2.elf`、`F4_LVGL_DEMO2.map`，以及用于代码索引的 `compile_commands.json`。

顶层 CMake 为应用目标追加 Debug `-Og`、Release `-O2`；工具链文件同时设置了 Debug `-O0`、Release `-Os`。分析具体文件的优化选项时，以实际编译命令为准，HAL / FreeRTOS 独立目标与应用目标的选项可能不同。

当前脚本没有自动生成 HEX / BIN，可按需手动转换：

```powershell
arm-none-eabi-objcopy -O ihex build/Debug/F4_LVGL_DEMO2.elf build/Debug/F4_LVGL_DEMO2.hex
arm-none-eabi-objcopy -O binary build/Debug/F4_LVGL_DEMO2.elf build/Debug/F4_LVGL_DEMO2.bin
arm-none-eabi-size build/Debug/F4_LVGL_DEMO2.elf
```

可使用 ST-LINK 与 STM32CubeProgrammer 烧录。ELF / HEX 携带地址信息；烧录原始 BIN 时，当前链接脚本对应起始地址为 `0x08000000`。仓库没有配置统一的 CMake 烧录目标，调试器连接和复位方式需按实际硬件设置。

## 7. 修改入口与注意事项

| 修改内容 | 主要文件 |
| --- | --- |
| 按钮样式、文字、事件与新界面 | `lvgl/App_UI/Src/ui.c` |
| 启动时创建的界面与任务 | `Core/Src/freertos.c` |
| LCD 引脚与方向 | `BSP/LCD/Inc/lcd_init.h` |
| LCD 初始化命令 | `BSP/LCD/Src/lcd_init.c` |
| 像素传输与屏幕偏移 | `BSP/LCD/Src/lcd.c` |
| LVGL 分辨率、缓冲、刷新回调 | `lvgl/porting/lv_port_lcd_stm32.c` |
| 触摸坐标标定与输入适配 | `lvgl/porting/lv_port_indev.c` |
| LVGL 日志、内存池、组件与监视器 | `lvgl/lv_conf.h` |
| RTOS tick、堆与内核选项 | `Core/Inc/FreeRTOSConfig.h` |
| 外设与时钟生成配置 | `F4_LVGL_DEMO2.ioc` |

- **统一屏幕尺寸：** 显示移植文件顶部保留了 `MY_DISP_VER_RES=320`，实际创建设备使用 280，触摸范围也为 280。修改尺寸时应统一实际调用、宏、触摸范围与 BSP 偏移。
- **横竖屏联动：** 仅修改 `USE_HORIZONTAL` 不足以完成 LVGL 旋转，还需同步显示分辨率、触摸坐标映射和像素偏移。
- **RGB565 字节序：** 当前由 `RGB565_SWAPPED` 配合 8 bit SPI 发送，避免再次交换像素字节。
- **异步刷新：** 若改用 DMA，应在传输完成后通知 LVGL 刷新结束，同时核对 DMA 数据对齐、SPI 数据宽度与缓冲生命周期。现有 DMA 配置为半字对齐，不能直接假定适配当前字节流。
- **刷新长度：** `LCD_MyFill()` 长度参数为 `uint16_t`；当前 13440 字节缓冲在其范围内。扩大缓冲或全屏发送时，需要重新检查长度并按需分段。
- **任务时基：** 当前 RTOS tick 为 1 ms，因此直接把 LVGL 返回的毫秒值传给 `vTaskDelay()` 可按此配置理解；修改 tick 频率后应进行单位转换。
- **并发访问：** 当前 UI 操作集中在 LVGL 任务。新增其他任务操作 LVGL 时，应按 LVGL 的线程同步机制保护调用。
- **触摸初始化返回值：** 输入适配层目前忽略 `cst816t_init()` 返回值，排障时可检查返回码、I²C 应答和芯片 ID。驱动在置位 `initialized` 前调用版本读取，而版本读取要求该标志已置位，固件版本读取顺序也待完善。
- **背光接口：** `LCD_Set_Light()`、`LCD_Close_Light()`、`LCD_Open_Light()` 中 PWM 代码被注释，当前未实现亮度调节。
- **CubeMX 重新生成：** 修改外设时同步维护 `.ioc`；自定义代码尽量放在 `USER CODE` 区域或独立 BSP / UI 文件，重新生成后检查差异。
- **新增源文件：** 顶层使用 `file(GLOB...)` 收集 BSP、LVGL 与 UI 源码，新增文件后应重新执行 CMake 配置。

## 8. 常见问题

| 现象 | 优先检查 |
| --- | --- |
| CMake 找不到编译器或 Ninja | 工具是否加入 `PATH`，是否使用项目 GCC 预设，旧缓存是否指向其他工具链 |
| 背光亮但没有界面 | SPI 接线、DC / CS / RST、屏幕初始化参数、时钟和 `App_lvgl()` 是否运行 |
| 图像上下偏移或裁切 | 240 × 280 可见区域、竖屏 Y+20 偏移以及实际模组要求是否一致 |
| 颜色异常 | RGB565 字节序、SPI 数据宽度与屏幕颜色配置 |
| 触摸无响应 | PB6 / PB7、I²C 地址 `0x15`、PB8 输出初始化、复位和驱动初始化返回值 |
| 触摸位置不准 | 原始坐标范围 222 / 280、横竖屏映射与实际显示尺寸 |
| 点击后没有串口日志 | `LV_USE_LOG=0`；启用后还需设置允许 USER 日志的级别，并接入日志输出回调或串口重定向 |
| 扩展界面后分配失败 | LVGL 32 KiB 内存池、FreeRTOS 15 KiB 堆、任务栈及链接 map 中的 RAM 占用 |

## 9. 第三方代码

工程包含 STM32 HAL / CMSIS、FreeRTOS、LVGL 及其附带组件。使用、分发或移植时，应分别核对相应组件中的许可证及源文件声明；本文不替代第三方许可证。
