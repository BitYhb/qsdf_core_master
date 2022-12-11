macro(__FindQtAndCheckVersion_find_qt)
    find_package(Qt6 REQUIRED ${QSDF_REQUIRED_QT_MODULES})
    set(_error_details "You probably need to set the Qt6_DIR variable.")
    if (NOT ${Qt6_FOUND})
        message(FATAL_ERROR "error: Qt ${QSDF_REQUIRED_QT_VERSION} was not found on your system. ${_error_details}")
    endif ()

    # Check Version
    if (Qt6_VERSION VERSION_LESS QSDF_REQUIRED_QT_VERSION)
        message("error: The required Qt version is at least ${QSDF_REQUIRED_QT_VERSION}, you cannot use Qt ${Qt6_VERSION}")
    endif ()
endmacro()

set(expected_defined_vars QSDF_REQUIRED_QT_VERSION QSDF_REQUIRED_QT_MODULES)
foreach (var ${expected_defined_vars})
    if (NOT DEFINED ${var})
        message(FATAL_ERROR "error: ${var} CMake variable is not defined.")
    endif ()
endforeach ()

__FindQtAndCheckVersion_find_qt()
