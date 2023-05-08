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

add_library(cclip SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/cclip.cpp
)
target_link_libraries(cclip
    clip
)
add_library(ctiktoken SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/ctiktoken.cpp
)
target_link_libraries(ctiktoken
    tiktoken
)
FILE(COPY 
        ${CMAKE_SOURCE_DIR}/cpp-tiktoken/cl100k_base.tiktoken
        ${CMAKE_SOURCE_DIR}/cpp-tiktoken/p50k_base.tiktoken
        ${CMAKE_SOURCE_DIR}/cpp-tiktoken/r50k_base.tiktoken 
    DESTINATION
        "${EXECUTABLE_OUTPUT_PATH}/tokenizers"
)
# copy Tokenizers to exe output path
add_library(utils STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.c
)
target_link_libraries(utils
    ezylog
    iniparser
    pthread
)
add_library(cdate STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cdate.c
)
add_library(cvector STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cvector.c
)