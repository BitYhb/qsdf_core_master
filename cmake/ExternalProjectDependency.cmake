if(NOT EXISTS "${EXTERNAL_PROJECT_DIR}")
    set(EXTERNAL_PROJECT_DIR ${CMAKE_SOURCE_DIR}/superbuild)
endif()

if(NOT DEFINED EXTERNAL_PROJECT_FILE_PREFIX)
    set(EXTERNAL_PROJECT_FILE_PREFIX "External_")
endif()

function(ExternalProject_Include_Dependencies target_name)
    set(options)
    set(oneValueArgs DEPENDS_VAR)
    set(multiValueArgs)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(_arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments: ${_arg_UNPARSED_ARGUMENTS}")
    endif()

    # set local variables
    set(_arg_DEPENDS ${${_arg_DEPENDS_VAR}})

    foreach(dep ${_arg_DEPENDS})
        if(EXISTS "${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
            include(${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake)
        endif()
    endforeach()
endfunction()
