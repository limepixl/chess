#!/bin/bash

cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/thirdparty/tinyobjloader -j 2
cmake --build build/thirdparty/SDL2 -j 2
cmake --build build -j 2
