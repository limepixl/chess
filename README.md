# Chess clone
Ова е едноставна имплементација на играта шах.

## Build
Наједноставен начин за компајлирање е со отварање на директориумот преку Visual Studio Code, користејќи ја CMake Tools екстензијата.

- Windows: MSVC
- Linux: GCC / Clang

За различни едитори или за компајлирање преку терминал, релативните paths не се тестирани затоа можно е да треба да ја преместите компајлираната програма во соодветна датотека.

- Visual Studio инструкции (потребен е инсталиран CMake на системот и нормална инсталација на Visual Studio)
1) Извршете ја batch скриптата build-cmake.bat
2) Би требало да се конфигурира проектот во build директориумот
3) Отворете го build директориумот
4) Отворете го Visual Studio Solution фајлот chess.sln
5) Најгоре, одберете Release наместо Debug
6) Стиснете најгоре Build -> Build Solution или пак стиснете F7 
7) Стиснете најгоре Local Windows Debugger или стиснете F5

## Third party libraries
- SDL2 (https://github.com/libsdl-org/SDL)
- GLAD (https://glad.dav1d.de/)
- GLM (https://github.com/g-truc/glm)
- stb_image.h (https://github.com/nothings/stb)
- tinyobjloader (https://github.com/tinyobjloader/tinyobjloader)
