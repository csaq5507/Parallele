cmake_minimum_required(VERSION 3.5)
project(Parallele)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(OpenCL REQUIRED)

IF(OPENCL_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCL_C_FLAGS}")
ENDIF()

FIND_PACKAGE(OpenMP REQUIRED)

IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pedantic -std=c11")

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

add_executable(mat_mul_wave mat_mul_wave.c)

target_include_directories (mat_mul_wave PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (mat_mul_wave ${OpenCL_LIBRARY} ${OpenMP_LIBRARY}  m)

# Scan through resource folder for updated files and copy if none existing or
file (GLOB_RECURSE resources "*.cl")
foreach(resource ${resources})
    get_filename_component(filename ${resource} NAME)
    get_filename_component(dir ${resource} DIRECTORY)
    get_filename_component(dirname ${dir} NAME)

    set (output "")

    while(NOT ${dirname} STREQUAL resources)
        get_filename_component(path_component ${dir} NAME)
        set (output "${path_component}/${output}")
        get_filename_component(dir ${dir} DIRECTORY)
        get_filename_component(dirname ${dir} NAME)
    endwhile()

    set(output "${CMAKE_CURRENT_BINARY_DIR}/${filename}")

    add_custom_command(
            COMMENT "Moving updated resource-file '${filename}'"
            OUTPUT ${output}
            DEPENDS ${resource}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${resource}
            ${output}
    )
    add_custom_target(${filename} ALL DEPENDS ${resource} ${output})

endforeach()
