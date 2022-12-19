cmake_minimum_required(VERSION 3.5)

project(FastIpConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FastIpConverter main.cpp
    src/FastIpConverter.h src/FastIpConverter.cpp)

include_directories(src)

install(TARGETS FastIpConverter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_compile_options(FastIpConverter PRIVATE -O3)

target_link_libraries(FastIpConverter PRIVATE -static)
