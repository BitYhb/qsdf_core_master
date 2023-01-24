set(SUPERBUILD_PREFIX "External_")
set(SUPERBUILD_FOLDER_NAME "superbuild")

set(DOMAIN_PROJECT_CMAKE_PREFIX "EPDomain_")

set(qsdf-core_DEPENDENCIES
    Lua)

ExternalProject_Include_Dependencies(qsdf-core DEPENDS_VAR qsdf-core_DEPENDENCIES)

if(QSDF_DOMAIN_NAME)
    set(QSDF_DOMAIN_SOURCE_DIRS ${SUPERBUILD_FOLDER_NAME}/domain)
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

set(proj ${PROJECT_NAME})

get_property(core_defines GLOBAL PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PRIVATE)
list(TRANSFORM core_defines REPLACE "\"" "\\\\\"" OUTPUT_VARIABLE core_defines)

get_property(core_public_defines GLOBAL PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PUBLIC)
list(TRANSFORM core_public_defines REPLACE "\"" "\\\\\"" OUTPUT_VARIABLE core_public_defines)

get_property(core_defines GLOBAL PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PRIVATE)
list(TRANSFORM core_defines REPLACE "\"" "\\\\\"" OUTPUT_VARIABLE core_defines)

get_property(core_public_defines GLOBAL PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PUBLIC)
list(TRANSFORM core_public_defines REPLACE "\"" "\\\\\"" OUTPUT_VARIABLE core_public_defines)

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
        -DQSDF_BUILD_SDK:BOOL=${QSDF_BUILD_SDK}
        -DCore_DELAY_ADD_COMPILE_DEFINITIONS_PRIVATE:STRING=${core_defines}
        -DCore_DELAY_ADD_COMPILE_DEFINITIONS_PUBLIC:STRING=${core_public_defines}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)

if(QSDF_DOMAIN_NAME)
    set(DOMAIN_PROJECT_NAME ${DOMAIN_PROJECT_CMAKE_PREFIX}${QSDF_DOMAIN_NAME})
    ExternalProject_Add_StepDependencies(${DOMAIN_PROJECT_NAME} build ${proj})
endif()
