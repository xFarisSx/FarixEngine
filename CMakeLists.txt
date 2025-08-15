
cmake_minimum_required(VERSION 3.15)
project(FarixEngine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE ENGINE_CPP "farixEngine/src/*.cpp")
set(GLAD_C "farixEngine/src/thirdparty/glad/glad.c")


add_library(farixEngine STATIC ${ENGINE_CPP} ${GLAD_C})


target_include_directories(farixEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/farixEngine/include>
        $<INSTALL_INTERFACE:include/farixEngine>
)


find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(farixEngine
    PUBLIC
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AssetConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/farixEngine/include/farixEngine/AssetConfig.h"
    @ONLY
)

target_include_directories(farixEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/farixEngine/include>
)


include(GNUInstallDirs)

install(TARGETS farixEngine
    EXPORT FarixEngineTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY farixEngine/include/farixEngine
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY farixEngine/assets
    DESTINATION ${CMAKE_INSTALL_DATADIR}/farixEngine
)

install(EXPORT FarixEngineTargets
    FILE FarixEngineTargets.cmake
    NAMESPACE Farix::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FarixEngine
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FarixEngineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FarixEngineConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FarixEngineConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FarixEngine
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FarixEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/FarixEngineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FarixEngine
)

