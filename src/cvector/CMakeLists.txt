cmake_minimum_required(VERSION 3.10)
project(cvector VERSION 1.0.5)

# cvector subproject

# VERSION LIST:
# * v1.0   - first version, achieve all basic function
# * v1.0.5 - add two debug function: 'cv_print_int' and 'cv_print_str'
#            they're used to output all objects in vectors

add_definitions(-DEXPORTING_APIS)

include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(. CVECTOR_SRC)
add_library(cvector SHARED
    ${CVECTOR_SRC}
)