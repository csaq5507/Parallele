cmake_minimum_required(VERSION 3.5)
project(Parallele)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenMP REQUIRED)

IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

FIND_PACKAGE(OpenCL REQUIRED)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenCL_CXX_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -O3 -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -O3 -std=c11")

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(week2_ocl mat_mul_ocl.c)
add_executable(week2_omp mat_mul_omp.c)
add_executable(week2_seq mat_mul_seq.c)

target_include_directories (week2_ocl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (week2_ocl ${OpenCL_LIBRARY})

target_include_directories (week2_omp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories (week2_seq PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
