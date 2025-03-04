License: MIT
Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
Email: supine0703@outlook.com
GitHub: https://github.com/supine0703
Repository: https://github.com/supine0703/lzl-cpp-lib

**This cmake configuration can be used for most c or cxx projects**

I put my usual **CMake** configuration on both **Linux** and **Windows**. While this is common to most projects, I've also wrapped some useful cmake functions to help me build more complex projects, including these at the top of the config whenever possible.

For example, you can use the following code to include this configuration in your project:

```cmake
# Set the cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Import the relevant cmake configuration
include(compiler_settings)
include(copy_lib_interface_headers)
include(copy_dirs_if_diff)
include(auto_install_post_build)
include(generate_lib_exports_header)
```

If you want to use the **Qt** configuration, you can add the following code: 

```cmake
# qt auto settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
```
