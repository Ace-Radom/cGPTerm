add_subdirectory(ezylog)
# build ezylog subproject
add_subdirectory(openai_api)
# build openai-api lib
add_subdirectory(rwcfg)
# build rwcfg lib
add_subdirectory(argparse)
# build argparse lib
add_subdirectory(cli)
# build cpp cli

add_library(utils
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.c
)
target_link_libraries(utils
    ezylog
    iniparser
)