# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Seleciona o board correto
set(PICO_BOARD pico_w CACHE STRING "Board type")

# SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

project(cap3un2 C CXX ASM)

pico_sdk_init()

add_executable(cap3un2 cap3un2.c)

pico_set_program_name(cap3un2 "cap3un2")
pico_set_program_version(cap3un2 "0.1")

# UART desativado / USB ativado
pico_enable_stdio_uart(cap3un2 0)
pico_enable_stdio_usb(cap3un2 1)

# Includes do projeto
target_include_directories(cap3un2 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Bibliotecas necessárias
target_link_libraries(cap3un2
    pico_stdlib
    hardware_adc
    hardware_gpio
    hardware_timer
    pico_cyw43_arch_lwip_threadsafe_background
)

pico_add_extra_outputs(cap3un2)
