cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME F4_LVGL_DEMO2)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Load the MCU toolchain before project() enables the compilers.
# Explicit toolchain selections from presets or the IDE remain supported.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc-arm-none-eabi.cmake"
        CACHE FILEPATH "STM32 toolchain file")
endif()

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

file(GLOB LCD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BSP/LCD/Src/*.c)
file(GLOB TOUCH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BSP/TOUCH/Src/*.c)
file(GLOB W28Q128_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BSP/W25Q128/Src/*.c)
file(GLOB_RECURSE LVGL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/*.c)
file(GLOB_RECURSE LVGL_PORT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/porting/*.c)
file(GLOB_RECURSE LVGL_UI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/App_UI/Src/*.c)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        ${LCD_SRC}
        ${TOUCH_SRC}
        ${W28Q128_SRC}
        ${LVGL_SRC}
        ${LVGL_PORT_SRC}
        ${LVGL_UI_SRC}
        Core/Src/test.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        BSP/LCD/Inc
        BSP/TOUCH/Inc
        BSP/W25Q128/Inc
        lvgl
        lvgl/App_UI/Inc
)

# ② 添加编译优化与分段选项
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -ffunction-sections
        -fdata-sections
        $<$<CONFIG:Debug>:-Og>          # Debug 用 -Og，比 -O0 小很多，仍可调试
        $<$<CONFIG:Release>:-O2>        # Release 用 -O2
)

# ③ 添加链接垃圾回收 + map 文件 + 内存统计
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map
        -Wl,--print-memory-usage
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
        LV_LVGL_H_INCLUDE_SIMPLE=1
        LV_CONF_INCLUDE_SIMPLE=1
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
