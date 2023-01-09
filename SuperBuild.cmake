set(SUPERBUILD_PREFIX "External_")
set(SUPERBUILD_FOLDER_NAME "superbuild")

set(qsdf-core_DEPENDENCIES
    Lua)

ExternalProject_Include_Dependencies(qsdf-core DEPENDS_VAR qsdf-core_DEPENDENCIES)

set(proj ${PROJECT_NAME})

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DEPENDS ${qsdf-core_DEPENDENCIES}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    BINARY_DIR ${CMAKE_BINARY_DIR}/${QSDF_BINARY_INNER_SUBDIR}
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-install
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_CACHE_ARGS
    -DQSDF_SUPERBUILD:BOOL=OFF
    -DQSDF_BUILD_SDK:BOOL=ON
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)

if(QSDF_DOMAIN_NAME)
    set(QSDF_DOMAIN_SOURCE_DIRS ${SUPERBUILD_FOLDER_NAME}/domain)
    set(DOMAIN_PROJECT_CMAKE_PREFIX "EPDomain_")
    foreach(domain_dir ${QSDF_DOMAIN_SOURCE_DIRS})
        cmake_path(ABSOLUTE_PATH domain_dir OUTPUT_VARIABLE domain_dir)
        cmake_path(GET domain_dir FILENAME domain_dir_name)
            if(EXISTS "${domain_dir}")
                # superbuild/domain
                unset(_domain_project_cmake_files CACHE)
                file(GLOB _domain_project_cmake_files RELATIVE "${domain_dir}" "${domain_dir}/${DOMAIN_PROJECT_CMAKE_PREFIX}*.cmake")
                foreach(_domain_project_cmake_file ${_domain_project_cmake_files})
                    include(${domain_dir}/${_domain_project_cmake_file})
                endforeach()
            endif()
    endforeach()
endif()
