# Chess clone
Ова е едноставна имплементација на играта шах.

## Build

- Windows: MSVC

НАПОМЕНА: Најпрво мора да имате инсталирано CMake (https://cmake.org) и Visual Studio (или Visual Studio Build Tools) **со модулот Desktop Development with C++**.

1) По инсталирање според горенаведените инструкции, во Windows Start Menu побарајте за Developer Command Prompt и извршете го првиот резултат.
2) Навигирајте до директориумот на репозиториумот што го преземавте.
3) Извршете ја batch скриптата build.bat со пишување на: build.bat
4) Откако ќе се компајлира цел проект, напишете во конзолата: `build\bin\chess.exe`

- Linux: GCC, Clang

НАПОМЕНА: Најпрво мора да имате инсталирано CMake и Ninja Build System. Ова се постигнува различно на секоја дистрибуција на Linux.

КОМАНДА ЗА UBUNTU: `sudo apt install cmake ninja-build gcc g++ clang xorg-dev libx11-dev libgl1-mesa-glx`

1) По инсталирање според горенаведените инструкции, отворете терминал и навигирајте до директориумот на репозиториумот што го преземавте.
2) Извршете ја shell скриптата build.sh (Ако не може да се изврши, напишете `chmod +x ./build.sh`)
3) Откако ќе се компајлира цел проект, напишете во терминалот: `./build/bin/chess`

## Third party libraries
- SDL2 (https://github.com/libsdl-org/SDL)
- GLAD (https://glad.dav1d.de/)
- GLM (https://github.com/g-truc/glm)
- stb_image.h (https://github.com/nothings/stb)
- tinyobjloader (https://github.com/tinyobjloader/tinyobjloader)
