cmake_minimum_required(VERSION 3.10)
project(rwcfg)

# rwcfg (read/write config) lib: read and write config.ini with libconfig library

aux_source_directory(. RWCFG_SRC)
add_library(rwcfg SHARED
    ${RWCFG_SRC}
)
target_link_libraries(rwcfg
    utils
    iniparser
    ezylog
)