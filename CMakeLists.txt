cmake_minimum_required(VERSION 3.10)
project(cGPTerm VERSION 1.0)

set(CMAKE_C_FLAGS "-Wall -g")
add_definitions(-D _DEFAULT_SOURCE)
add_definitions(-D _GNU_SOURCE)

set(LIBRARY_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/iniparser/src)
add_library(iniparser SHARED
    ${CMAKE_SOURCE_DIR}/iniparser/src/dictionary.c
    ${CMAKE_SOURCE_DIR}/iniparser/src/iniparser.c
)
include_directories(${CMAKE_SOURCE_DIR}/cpprich/include)
add_subdirectory(cpprich)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(cgpterm main.c ${UTILS})
target_link_libraries(cgpterm
    utils
    # ncursesw
    ezylog
    rwcfg
    argparse
    openaiapi
    cli
    cpprich
)