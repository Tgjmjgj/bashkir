cmake_minimum_required(VERSION 3.0.0)
project(bashkir VERSION 0.1.0)

set(EXE run)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

include_directories(includes)
file(GLOB_RECURSE SOURCES (src/*.cpp))

message("CMAKE BUILD TYPE is ${CMAKE_BUILD_TYPE}")

add_executable(${EXE} ${SOURCES})

target_compile_features(${EXE} PUBLIC cxx_std_17)
