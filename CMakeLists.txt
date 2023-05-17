cmake_minimum_required(VERSION 3.10)
project(cGPTerm)

add_compile_options(-fPIC)
option(BUILD_RELEASE OFF)
if(BUILD_RELEASE)
    set(CMAKE_C_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -O2")
else()
    set(CMAKE_C_FLAGS "-Wall -g")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -g")
endif()
add_definitions(-D_DEFAULT_SOURCE)
add_definitions(-D_GNU_SOURCE)

if(EXISTS ${CMAKE_SOURCE_DIR}/VERSION)
    file(READ ${CMAKE_SOURCE_DIR}/VERSION CGPTERM_VERSION)
    add_definitions(-DCGPTERM_VERSION="${CGPTERM_VERSION}")
else()
    message(FATAL "VERSION doesn't exist")
endif()

add_definitions(-DREAD_CURSOR_DELAY=10000)
# set cursor pos read delay to 10000us (10ms)
# if this definition not defined, cursor pos read delay will be 5000us (5ms)
# to add this definition is because, when setting delay to 5ms, sometimes it cannot read pos correctly
# if some errors like cannot use markdown + stream correctly occur, you may make the delay length longer here

set(LIBRARY_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/iniparser/src)
add_library(iniparser SHARED
    ${CMAKE_SOURCE_DIR}/iniparser/src/dictionary.c
    ${CMAKE_SOURCE_DIR}/iniparser/src/iniparser.c
)
include_directories(${CMAKE_SOURCE_DIR}/pthread_pool)
add_library(pthreadpool STATIC
    ${CMAKE_SOURCE_DIR}/pthread_pool/pthread_pool.c
)
include_directories(${CMAKE_SOURCE_DIR}/cpprich/include)
add_subdirectory(cpprich)
include_directories(${CMAKE_SOURCE_DIR}/clip)
add_subdirectory(clip)
include_directories(${CMAKE_SOURCE_DIR}/cpp-tiktoken)
add_subdirectory(cpp-tiktoken)

# build third party libraries

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(cGPTerm_Updater)
# build updater

add_executable(cgpterm main.c ${UTILS})
target_link_libraries(cgpterm
    utils
    ezylog
    rwcfg
    argparse
    openaiapi
    cli
    cpprich
    pthread
    curl
)