cmake_minimum_required(VERSION 3.5)
project(Parallele)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenCL REQUIRED)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pedantic -std=c11")

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(hillisteele hillissteele.c)

target_include_directories (hillisteele PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (hillisteele ${OpenCL_LIBRARY} m)


