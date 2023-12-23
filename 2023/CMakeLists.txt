cmake_minimum_required(VERSION 3.27)

project(advent_of_code_2023_CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")

include_directories(src/utils)

foreach(day RANGE 1 20)
    if(${day} LESS 10)
        set(day_prefix 0)
    else()
        set(day_prefix "")
    endif()
    add_executable(day${day_prefix}${day} src/day${day_prefix}${day}/main.cpp)
    install(TARGETS day${day_prefix}${day} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()