if(NOT DEFINED LIB_INTERFACE_HEADERS_INCLUDEDIR)
    set(LIB_INTERFACE_HEADERS_INCLUDEDIR ${CMAKE_SOURCE_DIR}/.include)
endif()
include_directories(${LIB_INTERFACE_HEADERS_INCLUDEDIR})

# Function to copy library interface headers
# Arguments:
# + LIB - (Library name) Directory to copy the headers to (Optional)
# + FROM - Directory to copy the headers from (Optional, defaults to the current source directory)
# + VERBOSE - Optional flag to enable verbose output (Optional)
# + HEADERS_FILES - List of header files to copy (Required)
# Note: COPY_LIB_INTERFACE_HEADERS_DISABLE_VERBOSE can be set to disable verbose output
function(copy_lib_i_headers)
    set(non_value_args VERBOSE)
    set(one_value_args LIB FROM)
    set(multi_value_args HEADERS_FILES)
    cmake_parse_arguments(ARG "${non_value_args}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT ARG_FROM)
        set(ARG_FROM ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(NOT ARG_HEADERS_FILES)
        message(FATAL_ERROR "HEADERS_FILES must be provided!")
    endif()

    set(VERBOSE ${ARG_VERBOSE})
    set(LIB ${ARG_LIB})
    set(FROM ${ARG_FROM})
    set(HEADERS_FILES ${ARG_HEADERS_FILES})

    foreach(file ${HEADERS_FILES})
        set(src_file "${FROM}/${file}")
        set(target_dir "${LIB_INTERFACE_HEADERS_INCLUDEDIR}")

        if(LIB)
            set(target_dir "${target_dir}/${LIB}")
        endif()

        if(NOT EXISTS ${src_file})
            message(FATAL_ERROR "File ${src_file} does not exist!")
        endif()

        # file(COPY ${src_file} DESTINATION ${target_dir})
        get_filename_component(file_name ${file} NAME)
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src_file} ${target_dir}/${file_name})

        if(VERBOSE AND NOT COPY_LIB_INTERFACE_HEADERS_DISABLE_VERBOSE)
            message(STATUS "Copied '${src_file}' to '${target_dir}/'")
        endif()
    endforeach()

    if(VERBOSE AND NOT COPY_LIB_INTERFACE_HEADERS_DISABLE_VERBOSE)
        message(STATUS)
    endif()
endfunction()
