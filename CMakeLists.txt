cmake_minimum_required(VERSION 3.0.0)
project(bashkir VERSION 0.1.0)

set(EXE run)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE BUILD TYPE is ${CMAKE_BUILD_TYPE}")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wcast-align -Wno-unused -Woverloaded-virtual -Wpedantic")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion -Wmisleading-indentation")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wduplicated-cond -Wduplicated-branches -Wlogical-op")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wnull-dereference -Wuseless-cast -Wdouble-promotion")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wformat=2 -Wwrite-strings")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(thirdparty/includes)
include_directories(includes)

file(GLOB_RECURSE SOURCES (src/*.cpp))

add_executable(${EXE} ${SOURCES})

target_compile_features(${EXE} PUBLIC cxx_std_17)

target_link_libraries(${EXE} ${CMAKE_SOURCE_DIR}/thirdparty/lib/ncurses/libncursesw_g.a)
target_link_libraries(${EXE} stdc++fs)

target_link_libraries(${EXE} ${CONAN_LIBS})