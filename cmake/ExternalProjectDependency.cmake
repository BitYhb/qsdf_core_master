if(NOT EXISTS "${EXTERNAL_PROJECT_DIR}")
    set(EXTERNAL_PROJECT_DIR ${CMAKE_SOURCE_DIR}/superbuild)
endif()

if(NOT DEFINED EXTERNAL_PROJECT_FILE_PREFIX)
    set(EXTERNAL_PROJECT_FILE_PREFIX "External_")
endif()

if(NOT DEFINED SUPERBUILD_TOPLEVEL_PROJECT)
    if(NOT DEFINED CMAKE_PROJECT_NAME)
        message(FATAL_ERROR "Failed to initialize variable SUPERBUILD_TOPLEVEL_PROJECT. Variable CMAKE_PROJECT_NAME is not defined.")
    endif()
    set(SUPERBUILD_TOPLEVEL_PROJECT ${CMAKE_PROJECT_NAME})
endif()

#
# ExternalProject_Include_Dependencies(<target_name>
#     [DEPENDS_VAR <depends_var>]
#     [EP_ARGS_VAR <external_project_args_var>]
# )
#
# DEPENDS_VAR Contains the name of the variable that contains the project dependency.
#             By default, it is `<target_name>_DEPENDS`
#
# EP_ARGS_VAR Name of the variable listing arguments to pass to ExternalProject.
#             If not specified, variable name default to `<project_name>_EP_ARGS`.
#
macro(ExternalProject_Include_Dependencies target_name)
    set(options)
    set(oneValueArgs DEPENDS_VAR EP_ARGS_VAR)
    set(multiValueArgs)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(_arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments: ${_arg_UNPARSED_ARGUMENTS}")
    endif()

    foreach(param DEPENDS EP_ARGS)
        if(NOT _sb_${param}_VAR)
            set(_sb_${param}_VAR ${target_name}_${param})
        endif()
    endforeach()

    # set local variables
    set(_arg_DEPENDS ${${_arg_DEPENDS_VAR}})

    foreach(dep ${_arg_DEPENDS})
        if(EXISTS "${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
            include(${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake)
        endif()
    endforeach()
endmacro()

function(_sb_extract_varname_and_vartype cmake_varname_and_type varname_var)
    set(_vartype_var "${ARGV2}")
    string(REPLACE ":" ";" varname_and_vartype ${cmake_varname_and_type})
    list(GET varname_and_vartype 0 _varname)
    list(GET varname_and_vartype 1 _vartype)
    set(${varname_var} ${_varname} PARENT_SCOPE)
    if(_vartype_var MATCHES ".+")
        set(${_vartype_var} ${_vartype} PARENT_SCOPE)
    endif()
endfunction()

function(mark_as_superbuild)
    set(options ALL_PROJECTS)
    set(oneValueArgs)
    set(multiValueArgs VARS PROJECTS LABELS)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(_arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments: ${_arg_UNPARSED_ARGUMENTS}")
    endif()

    if(_arg_PROJECTS AND _arg_ALL_PROJECTS)
        message(FATAL_ERROR "Arguments 'PROJECTS' and 'ALL_PROJECTS' are mutually exclusive!")
    endif()

    foreach(var ${_arg_VARS})
        set(_var ${var})
        get_property(_type_set_in_cache CACHE ${_var} PROPERTY TYPE SET)
        set(_var_name ${_var})
        set(_var_type "STRING")
        if(_type_set_in_cache)
            get_property(_var_type CACHE ${_var} PROPERTY TYPE)
        endif()
        set(_var ${_var_name}:${_var_type})
        list(APPEND _vars_with_type ${_var})
    endforeach()

    if(_arg_ALL_PROJECTS)
        set(optional_arg_ALL_PROJECTS "ALL_PROJECTS")
    else()
        set(optional_arg_ALL_PROJECTS PROJECTS ${_arg_PROJECTS})
    endif()

    _sb_append_to_cmake_args(VARS ${_vars_with_type} LABELS ${_arg_LABELS} ${optional_arg_ALL_PROJECTS})
endfunction()

function(_sb_append_to_cmake_args)
    set(options ALL_PROJECTS)
    set(oneValueArgs)
    set(multiValueArgs VARS PROJECTS LABELS)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(_arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments: ${_arg_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT _arg_ALL_PROJECTS AND NOT _arg_PROJECTS)
        set(_arg_PROJECTS ${SUPERBUILD_TOPLEVEL_PROJECT})
    endif()

    if(_arg_ALL_PROJECTS)
        set(_arg_PROJECTS ${_ALL_PROJECT_IDENTIFIER})
    endif()

    foreach(_arg_PROJECT ${_arg_PROJECTS})
        set(_ep_varnames "")
        foreach(varname_and_vartype ${_arg_VARS})
            if(NOT TARGET ${_arg_PROJECT})
                set_property(GLOBAL APPEND PROPERTY ${_arg_PROJECT}_EP_CMAKE_ARGS ${varname_and_vartype})
                _sb_extract_varname_and_vartype(${varname_and_vartype} _varname)
            else()
                message(FATAL_ERROR "Function _sb_append_to_cmake_args not allowed because project '${_arg_PROJECT}' already added !")
            endif()
            list(APPEND _ep_varnames ${_varname})
        endforeach()

        if(_arg_LABELS)
            set_property(GLOBAL APPEND PROPERTY ${_arg_PROJECT}_EP_LABELS ${_arg_LABELS})
            foreach(label ${_arg_LABELS})
                set_property(GLOBAL APPEND PROPERTY ${_arg_PROJECT}_EP_LABEL_${label} ${_ep_varnames})
            endforeach()
        endif()
    endforeach()
endfunction()
