cmake_minimum_required(VERSION 3.10)
project(ezylog VERSION 1.2)

# ezylog subproject: a thread-safe, lightweight, pure C implementation of a logging library

aux_source_directory(. EZYLOG_SRC)
add_library(ezylog SHARED
    ${EZYLOG_SRC}
)
target_link_libraries(ezylog
    m
    # libm: gcc math library
    pthread
)