if(NOT QSDF_DOMAIN_NAME STREQUAL "OKRDP")
    return()
endif()

set(proj EPDomain_OKRDP)
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
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DQSDF_SDK_PATH:PATH=${CMAKE_INSTALL_PREFIX}/sdk/${QSDF_CMAKE_INSTALL_PATH}/QSDF
        -DEPDomain_OKRDP_ENABLE_TESTS:BOOL=OFF
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/qsdf-core-build/bin)
