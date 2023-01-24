if(NOT QSDF_DOMAIN_NAME STREQUAL "okrdp")
    return()
endif()

set(proj EPDomain_okrdp)
set_property(GLOBAL APPEND PROPERTY QSDF_DOMAIN_PROJ_NAMES ${proj})
qsdf_delay_add_compile_definitions_to_plugin_core(
    PRIVATE
        QSDF_CURRENT_DOMAIN_PROJ="${proj}"
        QSDF_CURRENT_DOMAIN_PROJ_DLL_NAME="${proj}plugin.dll"
        QSDF_CURRENT_DOMAIN_PROJ_QML_MODULE_PREFER=\":/${proj}/\")

set(${proj}_DEPENDENCIES)

ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY
        git@deepliver-H470-HD:qt/qsdf_domains/epdomain_okrdp.git
    GIT_TAG origin/master
    CMAKE_CACHE_ARGS
        -DQSDF_SDK_PATH:PATH=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-install/${QSDF_CMAKE_INSTALL_PATH}/QSDF
        -DEPDomain_okrdp_ENABLE_TESTS:BOOL=OFF
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/qsdf-core-build/bin)
