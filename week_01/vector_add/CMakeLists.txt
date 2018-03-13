cmake_minimum_required(VERSION 3.5)
project(Parallele)

FIND_PACKAGE(OpenMP)

IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

FIND_PACKAGE(OpenCL)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenCL_CXX_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -O3 -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -O3 -std=c11")

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(week1_ocl vec_add_ocl.c)
add_executable(week1_omp vec_add_omp.c)
add_executable(week1_seq vec_add_seq.c)

target_include_directories (week1_ocl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week1_ocl ${OpenCL_LIBRARY})

target_include_directories (week1_omp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week1_omp ${OpenCL_LIBRARY})

target_include_directories (week1_seq PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week1_seq ${OpenCL_LIBRARY})