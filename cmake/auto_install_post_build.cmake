#[[ 
    License: MIT
    Copyright (c) 2024 李宗霖 (Li Zonglin)
    Email: supine0703@outlook.com
    GitHub: https://github.com/supine0703
]]

# Function to add a post build step to install the target
# Arguments:
# + TARGET - Name of the target to install (Required)
function(add_auto_install_post_build_if_enable TARGET)
    if(AUTO_INSTALL_POST_BUILD)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} --install ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
endfunction()
