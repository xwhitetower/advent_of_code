cmake_minimum_required(VERSION 3.27)

project(advent_of_code_2023_CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")

foreach(day RANGE 1 9 )
    if(${day} LESS 10)
        set(day_prefix 0)
    else()
        set(day_prefix "")
    endif()
    add_executable(day${day_prefix}${day}_1 src/day${day_prefix}${day}_1/main.cpp)
    add_executable(day${day_prefix}${day}_2 src/day${day_prefix}${day}_2/main.cpp)
    install(TARGETS day${day_prefix}${day}_1 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    install(TARGETS day${day_prefix}${day}_2 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()