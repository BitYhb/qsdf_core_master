# - Try to find Npm
# - Copied from one of cmake templates
# -------------------------
# Once done this will define
#
#  Npm_FOUND - system has Npm
#  Npm_EXECUTABLE - the Npm executable

set(_doc "Npm command line executable")

set(Npm_ROOT_PATH "C:/Program Files/nodejs" CACHE STRING "Npm executable path prefix")

set(Npm_NAMES npm)
if(WIN32)
    set(Npm_NAMES npm.cmd)
endif()

find_program(Npm_EXECUTABLE NAMES ${Npm_NAMES}
    PATHS ${Npm_ROOT_PATH}
    DOC ${_doc})
mark_as_advanced(Npm_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Npm
    FOUND_VAR Npm_FOUND
    REQUIRED_VARS Npm_EXECUTABLE)

if(Npm_FOUND)
    execute_process(COMMAND ${Npm_EXECUTABLE} --version OUTPUT_VARIABLE Npm_VERSION)
    find_package_check_version(${Npm_VERSION} in_range HANDLE_VERSION_RANGE
        RESULT_MESSAGE_VARIABLE reason)
    if (NOT in_range)
        message (FATAL_ERROR "${reason}")
    endif()
endif()

unset(_doc)
