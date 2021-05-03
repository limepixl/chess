cmake_minimum_required(VERSION 3.14.7)
project(chess)

add_subdirectory(thirdparty/SDL2)
set_target_properties(SDL2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(thirdparty/tinyobjloader)
include_directories(
	thirdparty/SDL2/include 
	thirdparty/glad/include
	thirdparty/glm
	thirdparty/stb_image
	thirdparty/tinyobjloader)

add_executable(
	chess 
	src/main.cpp 
	src/math/math.cpp
	src/event/event.cpp
	src/display/display.cpp
	src/asset_management/scene.cpp
	src/asset_management/texture.cpp
	src/asset_management/asset_management.cpp
	
	thirdparty/glad/src/glad.c
	thirdparty/stb_image/stb_image.c)

if(WIN32)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
		set(LIBS Setupapi.lib Winmm.lib Imm32.lib Version.lib ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/SDL2/SDL2d.lib)
	else(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
		set(LIBS Setupapi.lib Winmm.lib Imm32.lib Version.lib ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/SDL2/SDL2.lib)
	endif()

	

	target_link_libraries(${PROJECT_NAME} ${LIBS})
	if(MSVC)
		SET(CMAKE_CXX_FLAGS "/W4 /D_CRT_SECURE_NO_WARNINGS /EHsc")
	else()
		message("CURRENTLY UNSUPPORTED COMPILER ON WINDOWS!")
	endif()
	
elseif(UNIX)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
		set(LIBS ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/SDL2/libSDL2d.a pthread dl)
	else(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
		set(LIBS ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/SDL2/libSDL2.a pthread dl)
	endif()

	SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
	target_link_libraries(${PROJECT_NAME} ${LIBS})
endif()