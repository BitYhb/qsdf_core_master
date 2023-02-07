# - Try to find Npm
# - Copied from one of cmake templates
# -------------------------
# Once done this will define
#
#  Npm_FOUND - system has Npm
#  Npm_EXECUTABLE - the Npm executable

if(DEFINED CACHE{Npm_EXECUTABLE})
    set(Npm_FOUND True)
    return()
endif()

set(_doc "Npm command line executable")

set(Npm_ROOT_PATH "C:/Program Files/nodejs" CACHE STRING "Npm executable path prefix")

set(Npm_NAMES npm)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
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
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        cmake_path(GET Npm_EXECUTABLE FILENAME _npm_file)
        execute_process(COMMAND powershell "./${_npm_file}" --version
            WORKING_DIRECTORY ${Npm_ROOT_PATH}
            OUTPUT_VARIABLE Npm_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()

    find_package_check_version(${Npm_VERSION} in_range HANDLE_VERSION_RANGE
        RESULT_MESSAGE_VARIABLE reason)
    if(NOT in_range)
        message(FATAL_ERROR "${reason}")
    endif()
    message(STATUS "Found Npm: ${Npm_EXECUTABLE}, version: ${Npm_VERSION}")

    set(ENV{Path} "$ENV{Path};${Npm_ROOT_PATH}")
    set(Npm_EXECUTABLE "${Npm_EXECUTABLE}" CACHE STRING "" FORCE)

    #    execute_process(COMMAND ${Npm_EXECUTABLE} install --save-dev
    #        ERROR_QUIET
    #        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

unset(_doc)
