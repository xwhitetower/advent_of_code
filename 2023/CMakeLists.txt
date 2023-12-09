cmake_minimum_required(VERSION 3.27)

project(advent_of_code_2023_CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")

foreach(day RANGE 1 9 )
    add_executable(day${day}_1 src/day${day}_1/main.cpp)
    add_executable(day${day}_2 src/day${day}_2/main.cpp)
    install(TARGETS day${day}_1 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    install(TARGETS day${day}_2 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()