#[[
    License: GPL-3.0
    Copyright (c) 2024 李宗霖 (Li Zonglin)
    Email: supine0703@outlook.com
    GitHub: https://github.com/supine0703
    Repository: qt-settings <https://github.com/supine0703/qt-settings>
]]

project(lzl-qt-settings VERSION 0.3.1 LANGUAGES CXX)

set(SOURCES_FILES
    settings
    lzl_settings.cpp
    lzl_settings.h
    lzl_function_traits.h
    lzl_convert_qt_variant.h
    lzl_lib_settings_exports.h
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES_FILES}
)

target_link_libraries(${PROJECT_NAME}
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
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    add_auto_install_post_build_if_enable(${PROJECT_NAME})
endif()

# Copy library interface headers
copy_lib_i_headers(LIB lzl VERBOSE
    HEADERS_FILES
    settings
    lzl_settings.h
    lzl_function_traits.h
    lzl_convert_qt_variant.h
)

# Generate library exports header
generate_lib_exports_header(LIB lzl VERBOSE
    TARGET ${PROJECT_NAME}
    EXPORT_MACRO ${LZL_LIB_MACRO}
    FILE_NAME lzl_lib_settings_exports.h
)
