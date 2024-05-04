cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(temperature_read_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(TemperatureReading
    main.c
)
pico_enable_stdio_usb(TemperatureReading 1)

pico_add_extra_outputs(TemperatureReading)
target_link_libraries(TemperatureReading pico_cyw43_arch_none pico_stdlib hardware_adc)