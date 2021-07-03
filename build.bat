cmake -S thirdparty/SDL2/ -B thirdparty/build-SDL2 -DCMAKE_BUILD_TYPE=Release -G "Ninja" -DCMAKE_INSTALL_PREFIX=build
tools\ninja.exe -C thirdparty/build-SDL2/ install

cmake -S thirdparty/tinyobjloader -B thirdparty/build-TOBJL -DCMAKE_BUILD_TYPE=Release -G "Ninja" -DCMAKE_INSTALL_PREFIX=build
tools\ninja.exe -C thirdparty/build-TOBJL/ install

cmake -S . -B build-chess -DCMAKE_BUILD_TYPE=Release -G "Ninja" -DCMAKE_INSTALL_PREFIX=build
tools\ninja.exe -C build-chess/

move build-chess\chess.exe build\bin