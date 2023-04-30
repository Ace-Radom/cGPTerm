cmake_minimum_required(VERSION 3.10)
project(openai_api VERSION 1.0)

# openai_api subproject: requesting information from the openai api using the curl library

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_ARG build)
    set(TARGET_DIR "debug")
else()
    set(CARGO_ARG build --release)
    set(TARGET_DIR "release")
endif()
set(rstiktokens_a ${CMAKE_SOURCE_DIR}/src/openai_api/rstiktokens/target/${TARGET_DIR}/libtiktokens.a)

add_custom_target(rstiktokens ALL
    COMMAND cargo ${CARGO_ARG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/openai_api/rstiktokens
    COMMENT "Building Rust object rstiktokens"
)

aux_source_directory(. OPENAI_API_SRC)
add_library(openaiapi SHARED
    ${OPENAI_API_SRC}
)
add_dependencies(openaiapi rstiktokens)
target_link_libraries(openaiapi
    curl
    pthread
    pthreadpool
    jansson
    ezylog
    cpprich
    cdate
    cvector
    ${rstiktokens_a}
)
