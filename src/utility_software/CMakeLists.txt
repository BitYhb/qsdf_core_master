function(CONFIGURE_TOOLS_SUBMODULE target_name)
    set(_submodule_path ${CMAKE_CURRENT_SOURCE_DIR}/${target_name})
    if(EXISTS "${_submodule_path}" AND EXISTS "${_submodule_path}/CMakeLists.txt")
        add_subdirectory(${target_name})
    endif()
endfunction()

CONFIGURE_TOOLS_SUBMODULE(screendesigner)
CONFIGURE_TOOLS_SUBMODULE(buildoptions)
