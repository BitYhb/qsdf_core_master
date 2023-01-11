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
    HINTS ${Npm_ROOT_PATH}
    DOC ${_doc})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Npm
    FOUND_VAR Npm_FOUND
    REQUIRED_VARS Npm_EXECUTABLE)

unset(_doc)
