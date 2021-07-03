#!/bin/bash

# Build SDL2
cmake -S thirdparty/SDL2 -B thirdparty/build-SDL2 -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Ninja" -DCMAKE_INSTALL_PREFIX="build" -DSDL_STATIC="OFF" -DVIDEO_WAYLAND="OFF" -DWAYLAND_SHARED="OFF"
ninja -C thirdparty/build-SDL2 install

# Build tinyobjloader
cmake -S thirdparty/tinyobjloader -B thirdparty/build-TOBJL -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Ninja" -DCMAKE_INSTALL_PREFIX="build"
ninja -C thirdparty/build-TOBJL install

# Build chess
cmake -S . -B build-chess -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Ninja" -DCMAKE_INSTALL_PREFIX="build"
ninja -C build-chess

mv build-chess/chess build/bin/chess