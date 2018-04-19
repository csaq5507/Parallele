cmake_minimum_required(VERSION 3.5)
project(Parallele)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenCL REQUIRED)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenCL_CXX_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -O3 -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -O3 -std=c11")

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(week3_ocl heat_stencil_ocl.c)
add_executable(week3_ocl_locmem heat_stencil_ocl_locmem)

target_include_directories (week3_ocl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week3_ocl ${OpenCL_LIBRARY})

target_include_directories (week3_ocl_locmem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week3_ocl_locmem ${OpenCL_LIBRARY})



