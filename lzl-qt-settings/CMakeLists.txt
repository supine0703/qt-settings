project(lzl-qt-settings VERSION 0.3.6 LANGUAGES CXX)

# 检查目标是否存在
if(TARGET ${PROJECT_NAME})
    message(STATUS "Target '${PROJECT_NAME}' already exists.")
    return()
endif()

# 检测 C++ 标准
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 17)
elseif(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "lzl-qt-settings requires at least C++17 standard.")
endif()

set(LIB_EXPORT_HEADER lzl_lib_settings_exports.h)
set(INTERFACE_HEADERS
    settings
    function_traits
    lzl_settings.h
    lzl_convert_qt_variant.h
)
set(SOURCES_FILES
    ${INTERFACE_HEADERS}
    ${LIB_EXPORT_HEADER}
    lzl_settings.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
)

set(LZL_LIB_MACRO LZL_QT_SETTINGS)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:${LZL_LIB_MACRO}_STATIC>
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${LZL_LIB_MACRO}_SHARED>
    ${LZL_LIB_MACRO}_LIBRARY
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install
if(INSTALL_LZL_QT_SETTINGS_LIB)
    install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(INSTALL_INTERFACE_HEADERS)
        install(FILES
            ${INTERFACE_HEADERS}
            ${LIB_EXPORT_HEADER}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        )
    endif()

    add_auto_install_post_build_if_enable(${PROJECT_NAME})
endif()

# Copy library interface headers
copy_lib_i_headers(LIB lzl VERBOSE
    HEADERS_FILES ${INTERFACE_HEADERS}
)
copy_lib_i_headers(LIB lzl/functional/cxx17 VERBOSE
    HEADERS_FILES functional/cxx17/function_traits.h
)

# Generate library exports header
generate_lib_exports_header(LIB lzl VERBOSE
    TARGET ${PROJECT_NAME}
    EXPORT_MACRO ${LZL_LIB_MACRO}
    FILE_NAME ${LIB_EXPORT_HEADER}
)
