if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL CMAKE_SYSTEM_NAME)
    get_filename_component(_qt_find_root_path "${Qt6_DIR}/../../.." ABSOLUTE)
    list(APPEND CMAKE_FIND_ROOT_PATH ${_qt_find_root_path})
    mark_as_superbuild(VARS CMAKE_FIND_ROOT_PATH:PATH ALL_PROJECTS)
endif()

macro(__FindQtAndCheckVersion_find_qt)
    find_package(Qt6 ${QSDF_QT_VERSION_MIN} COMPONENTS ${QSDF_REQUIRED_QT_MODULES} REQUIRED)
    set(_error_details "You probably need to set the Qt6_DIR variable.")
    if(NOT ${Qt6_FOUND})
        message(FATAL_ERROR "error: Qt ${QSDF_REQUIRED_QT_VERSION} was not found on your system. ${_error_details}")
    endif()

    # Check Version
    if(Qt6_VERSION VERSION_LESS QSDF_REQUIRED_QT_VERSION)
        message("error: The required Qt version is at least ${QSDF_REQUIRED_QT_VERSION}, you cannot use Qt ${Qt6_VERSION}")
    endif()

    cmake_path(GET Qt6_DIR ROOT_PATH QTDIR)
    cmake_path(APPEND QTDIR "Qt/Tools/QtInstallerFramework/4.5/bin" OUTPUT_VARIABLE QTDIR)
    set(CPACK_IFW_ROOT ${QTDIR} CACHE STRING "" FORCE)
    mark_as_advanced(CPACK_IFW_ROOT)
endmacro()

set(expected_defined_vars QSDF_REQUIRED_QT_VERSION QSDF_REQUIRED_QT_MODULES)
foreach(var ${expected_defined_vars})
    if(NOT DEFINED ${var})
        message(FATAL_ERROR "error: ${var} CMake variable is not defined.")
    endif()
endforeach()

__FindQtAndCheckVersion_find_qt()
