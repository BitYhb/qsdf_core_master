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

function(_sb_cmakevar_to_cmakearg cmake_varname_and_vartype cmake_arg_var)
    _sb_extract_varname_and_vartype(${cmake_varname_and_vartype} _varname _vartype)
    set(_var_value ${${_varname}})
    get_property(_value_set_in_cache CACHE ${_varname} PROPERTY VALUE SET)
    if(_value_set_in_cache)
        get_property(_var_value CACHE ${_varname} PROPERTY VALUE)
    endif()

    set(${cmake_arg_var} -D${_varname}:${_vartype}=${_var_value} PARENT_SCOPE)
endfunction()

function(_sb_get_external_project_arguments proj varname)
    mark_as_advanced(${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj}:BOOL)

    function(_sb_collect_args proj)
        get_property(_labels GLOBAL PROPERTY ${proj}_EP_LABELS)
        if(_labels)
            list(REMOVE_DUPLICATES _labels)
            foreach(label ${_labels})
                get_property(${proj}_EP_LABEL_${label} GLOBAL PROPERTY ${proj}_EP_LABEL_${label})
                if(${proj}_EP_LABEL_${label})
                    list(REMOVE_DUPLICATES ${proj}_EP_LABEL_${label})
                endif()
                _sb_append_to_cmake_args(PROJECTS ${proj} VARS ${proj}_EP_LABEL_${label}:STRING)
            endforeach()
        endif()

        get_property(_cmake_args GLOBAL PROPERTY ${proj}_EP_CMAKE_ARGS)
        foreach(var ${_cmake_args})
            _sb_cmakevar_to_cmakearg(${var} _cmake_arg)
            set(_ep_property "CMAKE_CACHE_ARGS")
            set_property(GLOBAL APPEND PROPERTY ${proj}_EP_PROPERTY_${_ep_property} ${_cmake_arg})
        endforeach()
    endfunction()

    _sb_collect_args(${proj})

    set(_ep_arguments "")
    foreach(property CMAKE_CACHE_ARGS)
        get_property(${proj}_EP_PROPERTY_${property} GLOBAL PROPERTY ${proj}_EP_PROPERTY_${property})
        set(_all ${${proj}_EP_PROPERTY_${property}})
        list(LENGTH _all _num_properties)
        if(_num_properties GREATER 0)
            list(APPEND _ep_arguments ${property} ${_all})
        endif()
    endforeach()

    set(${varname} ${_ep_arguments} PARENT_SCOPE)
endfunction()

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

    set(_sb_proj ${target_name})

    # set local variables
    set(_sb_DEPENDS ${${_arg_DEPENDS_VAR}})

    foreach(param DEPENDS EP_ARGS)
        if(NOT _sb_${param}_VAR)
            set(_sb_${param}_VAR ${_sb_proj}_${param})
        endif()
    endforeach()

    # Save variables
    set_property(GLOBAL PROPERTY SB_${_sb_proj}_DEPENDS ${_sb_DEPENDS})
    set_property(GLOBAL PROPERTY SB_${_sb_proj}_EP_ARGS_VAR ${_sb_EP_ARGS_VAR})

    foreach(dep ${_sb_DEPENDS})
        get_property(_included GLOBAL PROPERTY SB_${_sb_proj}_FILE_INCLUDED)
        if(NOT _included)
            if(EXISTS "${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
                include(${EXTERNAL_PROJECT_DIR}/${EXTERNAL_PROJECT_FILE_PREFIX} ${dep}.cmake)
            else()
                message(FATAL_ERROR "error: Can't find ${EXTERNAL_PROJECT_FILE_PREFIX}${dep}.cmake")
            endif()
            set_property(GLOBAL PROPERTY SB_${_sb_proj}_FILE_INCLUDED 1)
        endif()
    endforeach()

    _sb_get_external_project_arguments(${_sb_proj} ${_sb_EP_ARGS_VAR})
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

    foreach(_proj ${_arg_PROJECTS})
        set(_ep_varnames "")
        foreach(varname_and_vartype ${_arg_VARS})
            if(NOT TARGET ${_proj})
                set_property(GLOBAL APPEND PROPERTY ${_proj}_EP_CMAKE_ARGS ${varname_and_vartype})
                _sb_extract_varname_and_vartype(${varname_and_vartype} _varname)
            else()
                message(FATAL_ERROR "Function _sb_append_to_cmake_args not allowed because project '${_proj}' already added!")
            endif()
            list(APPEND _ep_varnames ${_varname})
        endforeach()

        if(_arg_LABELS)
            set_property(GLOBAL APPEND PROPERTY ${_proj}_EP_LABELS ${_arg_LABELS})
            foreach(label ${_arg_LABELS})
                set_property(GLOBAL APPEND PROPERTY ${_proj}_EP_LABEL_${label} ${_ep_varnames})
            endforeach()
        endif()
    endforeach()
endfunction()
