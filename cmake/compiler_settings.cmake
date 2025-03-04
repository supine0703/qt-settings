# This file should be included at the beginning whenever possible

# enable CMake export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c++ and c standard
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# compiler and linker flags
function(add_flag_if_missing flag variable)
    string(FIND "${${variable}}" "${flag}" _pos)

    if(${_pos} EQUAL -1)
        set(${variable} "${flag} ${${variable}}" PARENT_SCOPE)
    endif()
endfunction()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # 添加 /utf-8 编译标志
    add_flag_if_missing("/utf-8" CMAKE_C_FLAGS)
    add_flag_if_missing("/utf-8" CMAKE_CXX_FLAGS)

    if(WARN_ALL)
        # 添加 /W4 编译标志
        add_flag_if_missing("/W4" CMAKE_C_FLAGS)
        add_flag_if_missing("/W4" CMAKE_CXX_FLAGS)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 添加 -Wl,--no-undefined 和 -Wl,--as-needed 链接器标志
    add_flag_if_missing("-Wl,--no-undefined" CMAKE_SHARED_LINKER_FLAGS)
    add_flag_if_missing("-Wl,--as-needed" CMAKE_SHARED_LINKER_FLAGS)
    add_flag_if_missing("-Wl,--no-undefined" CMAKE_MODULE_LINKER_FLAGS)
    add_flag_if_missing("-Wl,--as-needed" CMAKE_MODULE_LINKER_FLAGS)

    if(WARN_ALL)
        # 添加 -Wall 和 -Wextra 编译标志
        add_flag_if_missing("-Wall" CMAKE_C_FLAGS)
        add_flag_if_missing("-Wextra" CMAKE_C_FLAGS)
        add_flag_if_missing("-Wall" CMAKE_CXX_FLAGS)
        add_flag_if_missing("-Wextra" CMAKE_CXX_FLAGS)
    endif()
endif()

# visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # clangd check will error if here is set
    # set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

# unix-like RPATH
if(CMAKE_INSTALL_RPATH)
    foreach(_path ${CMAKE_INSTALL_RPATH})
        list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${_path}" _is_system_platform_lib_dir)
        list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${_path}" _is_system_cxx_lib_dir)

        if("${_is_system_platform_lib_dir}" STREQUAL "-1" AND "${_is_system_cxx_lib_dir}" STREQUAL "-1")
            set(CMAKE_SKIP_BUILD_RPATH FALSE)
            set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
            set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
            break()
        endif()
    endforeach()
endif()

# compiler info
set(C_COMPILER_INFO "${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
set(CXX_COMPILER_INFO "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# system info
set(SYSTEM_INFO "${CMAKE_SYSTEM_PROCESSOR} ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")

# timezone
set(TIMEZONE "")
find_program(PYTHON_EXECUTABLE NAMES python python3)

if(PYTHON_EXECUTABLE)
    # Find `get_timezone.py`
    if(NOT GET_TIMEZONE_PY)
        # 遍历CMAKE_MODULE_PATH中的每个目录
        foreach(MODULE_DIR ${CMAKE_MODULE_PATH})
            if(EXISTS "${MODULE_DIR}/get_timezone.py")
                set(GET_TIMEZONE_PY ${MODULE_DIR}/get_timezone.py)
                break()
            endif()
        endforeach()

        if(NOT GET_TIMEZONE_PY)
            message(FATAL_ERROR "get_timezone.py not found")
        endif()
    endif()

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${GET_TIMEZONE_PY}
        OUTPUT_VARIABLE TIMEZONE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Timezone: ${TIMEZONE}")
    set(TIMEZONE "${TIMEZONE} ")
else()
    message(WARNING "Python executable not found. Can't get timezone.")
endif()

# current time
string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")
set(CURRENT_TIME "${TIMEZONE}${CURRENT_TIME}")
