set(INTERNAL_SUPERBUILD_PREFIX "External_")
set(INTERNAL_SUPERBUILD_FOLDER_NAME "superbuild/internal")

if(NOT DEFINED QSDF_DOMAIN_NAME)
    message(FATAL_ERROR "error: Variable QSDF_DOMAIN_NAME is not defined!")
endif()

function(_isb_get_domain_source_path domain_name_var source_path)
    set(_domain_root_path "src/domains")
    cmake_path(ABSOLUTE_PATH ${_domain_root_path} NORMALIZE OUTPUT_VARIABLE _domain_root_path)

    set(source_path "${_domain_root_path}/${${domain_name_var}}" PARENT_SCOPE)
endfunction()

function(_ExternalProject_Add_Internal_SuperBuild domain_name)
    set(_prefix_var INTERNAL_SUPERBUILD_PREFIX)
    set(_isb_folder_relative_path_var INTERNAL_SUPERBUILD_FOLDER_NAME)
    string(TOLOWER "${domain_name}" _domain_name_lower)

    cmake_path(ABSOLUTE_PATH ${_isb_folder_relative_path_var} NORMALIZE OUTPUT_VARIABLE _isb_folder_absolute_path)
    set(_isb_file_absolute_path "${_isb_folder_absolute_path}/${_prefix_var}${_domain_name_lower}\.cmake")
    if(NOT EXISTS ${_isb_file_absolute_path})
        message(FATAL_ERROR "error: domain '${QSDF_DOMAIN_NAME}' is undefined!")
    endif()

    set(_proj ${_domain_name_lower})
    _isb_get_domain_source_path(_domain_name_lower _proj_source_path)

    ExternalProject_Include_Dependencies(${_proj} DEPENDS_VAR QSDF_DEPENDENCIES)

    ExternalProject_Add(${_proj}
        ${${_proj}_EP_ARGS}
        SOURCE_DIR ${_proj_source_path})
endfunction()
