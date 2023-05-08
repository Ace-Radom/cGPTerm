cmake_minimum_required(VERSION 3.10)
project(cGPTerm_CLI)

aux_source_directory(. CLI_SRC)
add_library(cli SHARED
    ${CLI_SRC}
)
target_link_libraries(cli
    openaiapi
    ezylog
    cclip
    cvector
    utils
    pthread
    readline
    cpprich
)
