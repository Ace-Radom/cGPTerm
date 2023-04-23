add_subdirectory(ezylog)
# build ezylog subproject
add_subdirectory(openai_api)
# build openai-api subproject

set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils.c PARENT_SCOPE)