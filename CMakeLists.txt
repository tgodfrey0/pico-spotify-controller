cmake_minimum_required(VERSION 3.13)

#include(pico_sdk_import.cmake)
include(lib/pico-sdk/pico_sdk_init.cmake)

set(PROJECT_NAME spotifyController)

project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

pico_sdk_init()

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(lib/lwjson)

target_link_libraries(${PROJECT_NAME} 
	pico_cyw43_arch_lwip_threadsafe_background 
	pico_lwip_mbedtls
	pico_mbedtls
	pico_stdlib 
	lwjson
)

target_sources(${PROJECT_NAME} PRIVATE
	src/main.c
	src/network.c
	src/spotify.c
	src/base64.c
)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})

