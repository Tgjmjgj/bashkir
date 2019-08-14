cmake_minimum_required(VERSION 3.0.0)
project(bashkir VERSION 0.1.0)

set(EXE run)

include_directories(includes)
file(GLOB SOURCES src/*.c)

add_executable(${EXE} ${SOURCES})
