cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)

project(brildaq_nivisa)

file(GLOB SOURCES "src/*.cpp")

include(CTest) 

enable_testing()

add_executable(instrument ${SOURCES})

target_include_directories(instrument PRIVATE ${PROJECT_SOURCE_DIR}/include)