cmake_minimum_required(VERSION 3.5)
project(Parallele)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenMP REQUIRED)

IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
ENDIF()

FIND_PACKAGE(OpenCL REQUIRED)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pedantic -std=c11")

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
SET(HEADERS stb/image.h stb/image_write.h)

add_executable(auto_level auto_level.c ${HEADERS})

target_include_directories (auto_level PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} stb)
target_link_libraries (auto_level ${OpenCL_LIBRARY} ${OpenMP_LIBRARY} m)
