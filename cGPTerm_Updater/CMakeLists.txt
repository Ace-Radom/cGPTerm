cmake_minimum_required(VERSION 3.10)
project(cGPTerm_Updater)

add_definitions(-DUPDATER_VERSION="v1.2")

add_executable(cgpterm-update
    ${CMAKE_CURRENT_SOURCE_DIR}/updater.c
)
target_link_libraries(cgpterm-update
    curl
    jansson
)