if(NOT QSDF_DOMAIN_NAME STREQUAL "IKPAS")
    return()
endif()

set(proj EPDomain_IKPAS)
set_property(GLOBAL APPEND PROPERTY QSDF_DOMAIN_PROJ_NAMES ${proj})
qsdf_delay_add_compile_definitions_to_plugin_core(
    PRIVATE
        QSDF_CURRENT_DOMAIN_PROJ="${proj}"
        QSDF_CURRENT_DOMAIN_PROJ_DLL_NAME="${proj}plugin.dll"
        QSDF_CURRENT_DOMAIN_PROJ_QML_MODULE_PREFER=\":/${proj}/\")

# CMAKE_FIND_ROOT_PATH
set(_sdk_path ${QSDF_INSTALL_ROOT}/sdk/${QSDF_CMAKE_INSTALL_PATH}/QSDF)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    list(FIND CMAKE_FIND_ROOT_PATH ${_sdk_path} find_index)
    if(find_index EQUAL -1)
        list(APPEND CMAKE_FIND_ROOT_PATH ${_sdk_path})
    endif()
endif()

set(${proj}_DEPENDENCIES)

ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

set(_install_prefix $<IF:$<BOOL:ANDROID>,${QSDF_INSTALL_ROOT}/${QSDF_ANDROID_LIB_INSTALL_PATH},${QSDF_INSTALL_ROOT}>)
ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY git@deepliver-H470-HD:qt/qsdf_domains/epdomain_ikpas.git
    GIT_TAG master
    CMAKE_CACHE_ARGS
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DQSDF_SDK_PATH:PATH=${_sdk_path}
        -DEPDomain_IKPAS_ENABLE_TESTS:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${_install_prefix})

unset(_sdk_path)
unset(_install_prefix)