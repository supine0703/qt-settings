#[[ 
    License: MIT
    Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
    Email: supine0703@outlook.com
    GitHub: https://github.com/supine0703
]]

if(NOT DEFINED LIB_INTERFACE_HEADERS_TARGET_PATH)
    set(LIB_INTERFACE_HEADERS_TARGET_PATH ${CMAKE_SOURCE_DIR}/include)
endif()

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
        set(target_dir "${LIB_INTERFACE_HEADERS_TARGET_PATH}")

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

# Function to generate library exports headers
# Arguments:
# + TARGET - Target name (Required)
# + MACRO - Macro name (Required)
# + FILE_NAME - File name (Optional, defaults to 'exports.h')
# + VERBOSE - Optional flag to enable verbose output (Optional)
# Note: GENERATE_EXPORTS_HEADER_DISABLE_VERBOSE can be set to disable verbose output
function(generate_lib_exports_header)
    set(non_value_args VERBOSE)
    set(one_value_args LIB TARGET EXPORT_MACRO FILE_NAME)
    cmake_parse_arguments(ARG "${non_value_args}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT ARG_TARGET)
        message(FATAL_ERROR "TARGET must be provided!")
    endif()

    if(NOT ARG_EXPORT_MACRO)
        message(FATAL_ERROR "MACRO must be provided!")
    endif()

    if(NOT ARG_FILE_NAME)
        set(ARG_FILE_NAME "exports.h")
    endif()

    set(VERBOSE ${ARG_VERBOSE})
    set(LIB ${ARG_LIB})
    set(TARGET ${ARG_TARGET})
    set(EXPORT_MACRO ${ARG_EXPORT_MACRO}_)
    set(FILE_NAME ${ARG_FILE_NAME})

    # Set target file
    set(target_dir "${LIB_INTERFACE_HEADERS_TARGET_PATH}")

    if(LIB)
        set(target_dir "${target_dir}/${LIB}")
    endif()

    set(target_file "${target_dir}/${FILE_NAME}")

    # Get target type
    get_target_property(target_type ${TARGET} TYPE)

    # Set template file based on target type
    set(template_file "${CMAKE_SOURCE_DIR}/cmake")

    if(target_type STREQUAL "STATIC_LIBRARY")
        set(template_file "${template_file}/static_exports.h.in")
    elseif(target_type STREQUAL "SHARED_LIBRARY")
        set(template_file "${template_file}/shared_exports.h.in")
    else()
        message(FATAL_ERROR "Unsupported target type: ${target_type}")
    endif()

    # Configure template file
    configure_file(${template_file} ${target_file} @ONLY)

    if(VERBOSE AND NOT GENERATE_EXPORTS_HEADER_DISABLE_VERBOSE)
        message(STATUS "Generated '${target_file}'")
        message(STATUS)
    endif()
endfunction()
