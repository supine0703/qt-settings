#[[ 
    License: MIT
    Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
    Email: supine0703@outlook.com
    GitHub: https://github.com/supine0703
]]

# qt auto settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT USE_CLANGD)
    # clangd check will error if here is set
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

# unix-like RPATH
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_FULL_LIBDIR}" _is_system_platform_lib_dir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_FULL_LIBDIR}" _is_system_cxx_lib_dir)

if("${_is_system_platform_lib_dir}" STREQUAL "-1" AND "${_is_system_cxx_lib_dir}" STREQUAL "-1")
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# compiler info
set(C_COMPILER_INFO "${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
set(CXX_COMPILER_INFO "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# system info
set(SYSTEM_INFO "${CMAKE_SYSTEM_PROCESSOR} ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
