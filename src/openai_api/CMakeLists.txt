cmake_minimum_required(VERSION 3.10)
project(openai_api VERSION 1.0)

# openai_api subproject: requesting information from the openai api using the curl library

aux_source_directory(. OPENAI_API_SRC)
add_library(openaiapi SHARED
    ${OPENAI_API_SRC}
)
target_link_libraries(openaiapi
    curl
    pthread
    pthreadpool
    jansson
    ezylog
    cpprich
    cdate
    cvector
    ctiktoken
)
