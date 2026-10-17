cmake_minimum_required(VERSION 3.20)

project(NetForge C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NOTCURSES REQUIRED IMPORTED_TARGET notcurses)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.c")

add_executable(netforge ${SOURCES})

target_include_directories(netforge PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(netforge PRIVATE
    PkgConfig::NOTCURSES
)