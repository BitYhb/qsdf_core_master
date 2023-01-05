if(NOT QSDF_DOMAIN_NAME STREQUAL "okr")
    return()
endif()

set(proj EPDomain_okr)
set_property(GLOBAL APPEND PROPERTY QSDF_DOMAIN_PROJ_NAMES ${proj})

set(${proj}_DEPENDENCIES
    Lua)

ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

_sb_get_domain_source_path(proj _proj_source_path)

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DEPENDS qsdf-core
    GIT_REPOSITORY git@deepliver-H470-HD:qt/qsdf_domains/epdomain_okr.git
    GIT_TAG origin/master
    CMAKE_CACHE_ARGS
        -DQSDF_SDK_PATH:PATH=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-install/${QSDF_CMAKE_INSTALL_PATH}/QSDF
    INSTALL_COMMAND "")
