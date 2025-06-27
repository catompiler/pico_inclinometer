# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.20)

# pico SDK
#set(PICO_SDK_PATH ../../pico-sdk)
# pico platform
set(PICO_PLATFORM rp2040)
# pico board
set(PICO_BOARD pico)#waveshare_rp2040_lcd_1.28)
# pico compiler
set(PICO_COMPILER pico_arm_cortex_m0plus_gcc)

# fix for windows
if((CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows") OR (CMAKE_HOST_SYSTEM_NAME STREQUAL "CYGWIN"))
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
endif()

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

# project name
project(pico_inclinometer)

# standards
# C
set(CMAKE_C_STANDARD 17)
# C++
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# executable
add_executable(${PROJECT_NAME} main.c)

# defines
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_DEF=1)

# libraries
target_link_libraries(${PROJECT_NAME} pico_stdlib)

# pico SW FP.
pico_set_float_implementation(${PROJECT_NAME} pico)
pico_set_double_implementation(${PROJECT_NAME} none)
target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_FLOAT_PROPAGATE_NANS=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_DOUBLE_PROPAGATE_NANS=1)

# stdio
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# run from ram
#pico_set_binary_type(TARGET_NAME no_flash)

# extra outputs
pico_add_extra_outputs(${PROJECT_NAME})

# linker options
target_link_options(${PROJECT_NAME}
    PRIVATE "LINKER:--print-memory-usage"
)
