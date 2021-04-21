cmake_minimum_required(VERSION 3.14.7)
project(chess)

set(DUMMYAUDIO OFF)
set(DIRECTX OFF)
set(DISKAUDIO OFF)
set(RENDER_D3D OFF)
add_subdirectory(thirdparty/SDL2)
include_directories(
	thirdparty/SDL2/include 
	thirdparty/glad/include
	thirdparty/glm
	thirdparty/stb_image)

add_executable(
	chess 
	src/main.cpp 
	src/display/display.cpp
	src/asset_management/asset_management.cpp
	thirdparty/glad/src/glad.c
	thirdparty/stb_image/stb_image.c)

if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/SDL2/SDL2-staticd.lib Setupapi.lib Winmm.lib Imm32.lib Version.lib)
endif()