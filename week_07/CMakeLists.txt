cmake_minimum_required(VERSION 3.5)
project(Parallele)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pedantic -std=c11")

add_executable(gen_list list_gen.c people.h)
add_executable(sort_list list_sort.c people.h)

target_include_directories (gen_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (sort_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries (countsort m)
