set(SUPERBUILD_PREFIX "External_")
set(SUPERBUILD_FOLDER_NAME "superbuild")

#set(QSDF_DEPENDENCIES
#    Lua)

foreach(extension_dir ${QSDF_EXTENSION_SOURCE_DIRS})
    cmake_path(ABSOLUTE_PATH extension_dir OUTPUT_VARIABLE extension_dir)
    cmake_path(GET extension_dir FILENAME extension_name)
    if(EXISTS "${extension_dir}/${SUPERBUILD_FOLDER_NAME}")
        # superbuild
        unset(_external_project_cmake_files CACHE)
        file(GLOB _external_project_cmake_files RELATIVE "${extension_dir}/${SUPERBUILD_FOLDER_NAME}"
            "${extension_dir}/${SUPERBUILD_FOLDER_NAME}/${SUPERBUILD_PREFIX}*.cmake")
        foreach(_external_project_cmake_file ${_external_project_cmake_files})
            string(REGEX MATCH "${SUPERBUILD_PREFIX}(.+)\.cmake" _match_external_project_name ${_external_project_cmake_file})
        endforeach()
    endif()
endforeach()

ExternalProject_Include_Dependencies(${PROJECT_NAME} DEPENDS_VAR QSDF_DEPENDENCIES)

ExternalProject_Add(${PROJECT_NAME}
    ${${PROJECT_NAME}_EP_ARGS}
    DEPENDS ${QSDF_DEPENDENCIES}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    BINARY_DIR ${CMAKE_BINARY_DIR}/${QSDF_BINARY_INNER_SUBDIR}
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_CACHE_ARGS
        -DQSDF_SUPERBUILD:BOOL=OFF
    INSTALL_DIR "")
