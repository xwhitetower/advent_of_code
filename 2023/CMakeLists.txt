cmake_minimum_required(VERSION 3.27)

project(advent_of_code_2023_CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")

include_directories(src/utils)

foreach(day RANGE 1 11)
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


add_executable(day12_1 src/day12_1/main.cpp)
install(TARGETS day12_1 DESTINATION "${CMAKE_SOURCE_DIR}/bin")

# temporary until day 12 is fully done
foreach(day RANGE 13 18)
    add_executable(day${day_prefix}${day}_1 src/day${day_prefix}${day}_1/main.cpp)
    add_executable(day${day_prefix}${day}_2 src/day${day_prefix}${day}_2/main.cpp)
    install(TARGETS day${day_prefix}${day}_1 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    install(TARGETS day${day_prefix}${day}_2 DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()


# New format
foreach(day RANGE 19 19)
    add_executable(day${day_prefix}${day} src/day${day_prefix}${day}/main.cpp)
    install(TARGETS day${day_prefix}${day} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()