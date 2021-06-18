# Chess clone
Ова е едноставна имплементација на играта шах.

## Build
НАПОМЕНА: Најпрво мора да имате инсталирано CMake (https://cmake.org) и Visual Studio (или Visual Studio Build Tools) **со модулот Desktop Development with C++**.

- Windows: MSVC
- Linux: GCC / Clang

1) По инсталирање според горенаведените инструкции, во Windows Start Menu побарајте за Developer Command Prompt и извршете го првиот резултат.
2) Навигирајте до директориумот на репозиториумот што го преземавте.
3) Извршете ја batch скриптата build-cmake.bat со пишување на: build-cmake.bat
4) Откако ќе се компајлира цел проект, напишете во конзолата: build\bin\chess.exe

## Third party libraries
- SDL2 (https://github.com/libsdl-org/SDL)
- GLAD (https://glad.dav1d.de/)
- GLM (https://github.com/g-truc/glm)
- stb_image.h (https://github.com/nothings/stb)
- tinyobjloader (https://github.com/tinyobjloader/tinyobjloader)
