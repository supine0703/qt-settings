# Function to copy directories to a destination if they are different
# Arguments:
# + DESTINATION - Directory to copy the directories to (Required)
# + DIRECTORIES - List of directories to copy (Required)
# + VERBOSE - Optional flag to enable verbose output (Optional)
# Note: COPY_DIRS_IF_DIFF_DISABLE_VERBOSE can be set to disable verbose output
function(copy_dirs_if_diff)
    set(non_value_args VERBOSE)
    set(one_value_args DESTINATION)
    set(multi_value_args DIRECTORIES)
    cmake_parse_arguments(ARG "${non_value_args}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(VERBOSE ${ARG_VERBOSE})
    set(DESTINATION ${ARG_DESTINATION})
    set(DIRECTORIES ${ARG_DIRECTORIES})

    if(NOT DIRECTORIES)
        message(FATAL_ERROR "DIRECTORIES must be provided!")
    endif()

    # Convert destination to absolute path if it is relative
    if(NOT IS_ABSOLUTE ${DESTINATION})
        get_filename_component(DESTINATION "${DESTINATION}" ABSOLUTE)
    endif()

    foreach(dir ${DIRECTORIES})
        if(NOT EXISTS ${dir})
            message(FATAL_ERROR "Directory ${dir} does not exist!")
        endif()

        # Convert directory to absolute path if it is relative
        if(NOT IS_ABSOLUTE ${dir})
            get_filename_component(dir "${dir}" ABSOLUTE)
        endif()

        file(GLOB_RECURSE files ${dir}/*)

        foreach(file ${files})
            file(RELATIVE_PATH rel_path ${dir} ${file})
            get_filename_component(dir_name ${dir} NAME)
            set(target_file ${DESTINATION}/${dir_name}/${rel_path})

            # Copy the file if it is different
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} ${target_file})

            if(VERBOSE AND NOT COPY_DIRS_IF_DIFF_DISABLE_VERBOSE)
                message(STATUS "Copied '${file}' to '${target_file}' if different")
            endif()
        endforeach()
    endforeach()
endfunction()
