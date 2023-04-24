cmake_minimum_required(VERSION 3.10)
project(argparse)

# argparse lib: use argtable2 to parse args from main

aux_source_directory(. ARGPARSE_SRC)
add_library(argparse STATIC
    ${ARGPARSE_SRC}
)
target_link_libraries(argparse
    argtable2
)