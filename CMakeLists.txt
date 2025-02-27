cmake_minimum_required(VERSION 3.16)

project(qt-settings-demo VERSION 0.2.1)

# ...
option(WARN_ALL "Enable all warnings" ON)
option(BUILD_ONLY_LIBRARY "Build only library" OFF)
option(INSTALL_LZL_QT_SETTINGS_LIB "Install utils lzl settings lib" OFF)
option(COPY_DIRS_IF_DIFF_DISABLE_VERBOSE "Disable verbose output for copy_dirs_if_diff" ON)
option(COPY_LIB_INTERFACE_HEADERS_DISABLE_VERBOSE "Disable verbose output for copy_lib_interface_headers" ON)
option(GENERATE_EXPORTS_HEADER_DISABLE_VERBOSE "Disable verbose output for generate_lib_exports_header" ON)

# 设置 cmake 模块路径
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 查找 Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

# 引入相关 cmake 配置
include(compiler_settings)
include(copy_lib_interface_headers)
include(copy_dirs_if_diff)
include(auto_install_post_build)
include(GNUInstallDirs)

# 添加头文件路径
include_directories(${LIB_INTERFACE_HEADERS_TARGET_PATH})

# 添加 lzl-qt-settings 库
add_subdirectory(lzl-qt-settings)

if(BUILD_ONLY_LIBRARY)
    return()
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-settings
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET qt-settings APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qt-settings SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qt-settings
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(qt-settings PRIVATE
    CONFIG_INI="config.ini"
)

target_link_libraries(qt-settings PRIVATE Qt${QT_VERSION_MAJOR}::Widgets lzl-qt-settings)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qt-settings)
endif()

set_target_properties(qt-settings PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qt-settings
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-settings)
endif()
