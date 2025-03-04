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
