cmake_minimum_required(VERSION 3.10)
project(cGPTerm VERSION 1.0)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(main main.c)
target_link_libraries(main
    ${CURSES_LIBRARIES}
)