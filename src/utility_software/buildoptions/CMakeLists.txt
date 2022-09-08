unset(_buildoptions_header_files CACHE)
unset(_buildoptions_source_files CACHE)
unset(_buildoptions_ui_files CACHE)

file(GLOB _buildoptions_header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
file(GLOB _buildoptions_source_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")
file(GLOB _buildoptions_ui_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.ui")

unset(_ui_header_includes CACHE)
qt_wrap_ui(_ui_header_files ${_buildoptions_ui_files})
foreach(_ui_header_file ${_ui_header_files})
    cmake_path(GET _ui_header_file PARENT_PATH _ui_header_file_path)
    list(APPEND _ui_header_includes ${_ui_header_file_path})
endforeach()
list(REMOVE_DUPLICATES _ui_header_includes)

add_mips_executable(BuildOptionsTool
    DEPENDS
    PUBLIC_DEPENDS Qt6::Widgets
    SOURCES ${_buildoptions_header_files} ${_buildoptions_source_files} ${_buildoptions_ui_files}
    INCLUDES ${_ui_header_includes}
    MSVC_SOLUTION_FOLDER ${_TOOLS_SOLUTION_FOLDER}
)
