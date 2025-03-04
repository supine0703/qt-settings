# Function to generate library exports headers
# Arguments:
# + LIB - Library name (Optional)
# + TARGET - Target name (Required)
# + EXPORT_MACRO - Macro name (Required)
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
    set(EXPORT_MACRO ${ARG_EXPORT_MACRO})
    set(FILE_NAME ${ARG_FILE_NAME})

    # Set `OS_EXPORT` and `OS_IMPORT`
    if(WIN32)
        set(OS_EXPORT "__declspec(dllexport)")
        set(OS_IMPORT "__declspec(dllimport)")
    else()
        set(OS_EXPORT "__attribute__((visibility(\"default\")))")
        set(OS_IMPORT "__attribute__((visibility(\"default\")))")
    endif()

    # Set target file
    set(target_dir "${LIB_INTERFACE_HEADERS_INCLUDEDIR}")

    if(LIB)
        set(target_dir "${target_dir}/${LIB}")
    endif()

    set(target_file "${target_dir}/${FILE_NAME}")

    # Get target type
    get_target_property(target_type ${TARGET} TYPE)

    # Set template file
    if(target_type STREQUAL "STATIC_LIBRARY")
        set(template_file ${STATIC_LIBRARY_EXPORTS_H_IN})
    elseif(target_type STREQUAL "SHARED_LIBRARY")
        set(template_file ${SHARED_LIBRARY_EXPORTS_H_IN})
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

# Find `shared_exports.h.in` and `static_exports.h.in`
if(NOT STATIC_LIBRARY_EXPORTS_H_IN AND NOT SHARED_LIBRARY_EXPORTS_H_IN)
    # 遍历CMAKE_MODULE_PATH中的每个目录
    foreach(MODULE_DIR ${CMAKE_MODULE_PATH})
        if(EXISTS "${MODULE_DIR}/shared_exports.h.in")
            set(SHARED_LIBRARY_EXPORTS_H_IN ${MODULE_DIR}/shared_exports.h.in)
            message(STATUS "Find `SHARED_LIBRARY_EXPORTS_H_IN`: ${SHARED_LIBRARY_EXPORTS_H_IN}")
        endif()

        if(EXISTS "${MODULE_DIR}/static_exports.h.in")
            set(STATIC_LIBRARY_EXPORTS_H_IN ${MODULE_DIR}/static_exports.h.in)
            message(STATUS "Find `STATIC_LIBRARY_EXPORTS_H_IN`: ${STATIC_LIBRARY_EXPORTS_H_IN}")
        endif()

        if(STATIC_LIBRARY_EXPORTS_H_IN AND SHARED_LIBRARY_EXPORTS_H_IN)
            break()
        endif()
    endforeach()

    if(NOT STATIC_LIBRARY_EXPORTS_H_IN)
        message(FATAL_ERROR "static_exports.h.in not found!")
    elseif(NOT SHARED_LIBRARY_EXPORTS_H_IN)
        message(FATAL_ERROR "shared_exports.h.in not found!")
    endif()
endif()
