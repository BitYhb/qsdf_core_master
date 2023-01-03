set(proj EPDomain_okr)
set_property(GLOBAL APPEND PROPERTY QSDF_DOMAIN_PROJ_NAMES ${proj})

set(${proj}_DEPENDENCIES)

set(${proj}_SDK_DEPENDS Utils ExtensionSystem)

ExternalProject_Include_Dependencies(${proj}
    PROJECT_VAR proj
    DEPENDS_VAR ${proj}_DEPENDENCIES)

foreach(depend ${${proj}_SDK_DEPENDS})
    set_property(GLOBAL APPEND PROPERTY ${proj}_EP_SDK_DEPENDENCIES ${depend})
endforeach()

_sb_get_domain_source_path(proj _proj_source_path)

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY git@deepliver-H470-HD:qt/qsdf_domains/epdomain_okr.git
    GIT_TAG origin/master
    CMAKE_CACHE_ARGS
        -DQSDF_SDK_PATH:PATH=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-install/${QSDF_CMAKE_INSTALL_PATH}/QSDF
    INSTALL_COMMAND "")
