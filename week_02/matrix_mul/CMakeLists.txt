cmake_minimum_required(VERSION 3.5)
project(Parallele)


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

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(assignment3_ocl mat_mul_ocl.c)
add_executable(assignment3_omp mat_mul_omp.c)
add_executable(assignment3_seq mat_mul_seq.c)

target_include_directories (assignment3_ocl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (assignment3_ocl ${OpenCL_LIBRARY})

target_include_directories (assignment3_omp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories (assignment3_seq PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
