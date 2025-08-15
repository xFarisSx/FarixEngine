
cmake_minimum_required(VERSION 3.15)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/Builds/CppProjectBuild)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)

find_package(FarixEngine REQUIRED)

add_executable(mygame
    main.cpp
    game/game.cpp
)




target_link_libraries(mygame
    PRIVATE
        Farix::farixEngine
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
)

