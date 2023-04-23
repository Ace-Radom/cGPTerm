cmake_minimum_required(VERSION 3.10)
project(cGPTerm VERSION 1.0)

set(CMAKE_C_FLAGS "-Wall")

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(LIBRARY_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/iniparser/src)
add_library(iniparser SHARED
    ${CMAKE_SOURCE_DIR}/iniparser/src/dictionary.c
    ${CMAKE_SOURCE_DIR}/iniparser/src/iniparser.c
)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(main main.c ${UTILS})
target_link_libraries(main
    ${CURSES_LIBRARIES}
    ezylog
    rwcfg
)