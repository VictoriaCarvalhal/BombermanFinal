cmake_minimum_required(VERSION 3.27)
project(sdl2 C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "${CMAKE_SOURCE_DIR}/SDL2-2.30.3/x86_64-w64-mingw32")

set(SDL2TTF_PATH "${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/x86_64-w64-mingw32")
set(SDL2TTF_LIBRARY "mingw32;-mwindows;${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib/libSDL2_ttf.a;${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a")
set(SDL2TTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include/SDL2")

set(SDL2IMAGE_PATH "${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/x86_64-w64-mingw32")
set(SDL2IMAGE_LIBRARY "mingw32;-mwindows;${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/x86_64-w64-mingw32/lib/libSDL2_image.a;${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/x86_64-w64-mingw32/lib/libSDL2_image.dll.a")
set(SDL2IMAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/X86_64-w64-mingw32/include/SDL2")

find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)
find_package(SDL2IMAGE REQUIRED)

add_executable(sdl2 main.c)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2IMAGE_LIBRARY} Rpcrt4)