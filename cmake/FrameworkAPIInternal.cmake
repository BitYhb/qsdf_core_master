### 路径相关
if (WIN32)
    set(_QSDF_HEADER_INSTALL_PATH "include/qsdf")
    set(_QSDF_CMAKE_INSTALL_PATH "lib/cmake")

    set(_QSDF_LIBRARY_BASE_PATH "lib")
    set(_QSDF_DATA_PATH "share/qsdf")
    set(_QSDF_LIBRARY_PATH "${_QSDF_LIBRARY_BASE_PATH}/qsdf")
    set(_QSDF_PLUGIN_PATH "${_QSDF_LIBRARY_BASE_PATH}/qsdf/plugins")
    set(_QSDF_BIN_PATH "bin")
    set(_QSDF_LIBRARY_ARCHIVE_PATH ${_QSDF_BIN_PATH})
    set(_QSDF_LIBEXEC_PATH "bin")

    set(_RPATH_BASE "")
    set(_LIB_RPATH "")
    set(_PLUGIN_RPATH "")
endif()

if (WITH_USE_SOLUTION_FOLDERS)
    set(_APP_SOLUTION_FOLDER "AppTargets")
    set(_PLUGINS_SOLUTION_FOLDER "PluginTargets")
    set(_CORE_SOLUTION_FOLDER "CoreTargets")
    set(_SHARED_SOLUTION_FOLDER "SharedTargets")
    set(_TOOLS_SOLUTION_FOLDER "ToolsTargets")
    set(_OTHER_SOLUTION_FOLDER "OtherTargets")
endif()

### 设置版本以及项目有关信息
set(QSDF_VERSION "1.0.0")
set(QSDF_SETTINGSVARIANT "QSDF")
set(QSDF_DISPLAY_NAME "QSDF")
set(QSDF_ID "qsdf")
set(QSDF_CASED_ID "QSDF")

### Internal
set(__QSDF_TESTS "" CACHE INTERNAL "***** Internal *****")

# Must be called after project(...).
function(qsdf_handle_sccache_support)
    if (MSVC AND WITH_SCCACHE_SUPPORT)
        foreach(config DEBUG RELWITHDEBINFO)
            foreach(lang C CXX)
                set(flags_var "CMAKE_${lang}_FLAGS_${config}")
                string(REPLACE "/Zi" "/Z7" ${flags_var} "${${flags_var}}")
                set(${flags_var} "${${flags_var}}" PARENT_SCOPE)
            endforeach()
        endforeach()
    endif()
endfunction()

function(set_explicit_moc target_name file)
    unset(file_dependencies)
    if (file MATCHES "^.*plugin\.h$")
        set(file_dependencies DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.json")
    endif()
    set_property(SOURCE "${file}" PROPERTY SKIP_AUTOMOC ON)
    qt6_wrap_cpp(file_moc "${file}" ${file_dependencies})
    target_sources(${target_name} PRIVATE ${file_moc})
endfunction()

function(set_public_headers target_name sources)
endfunction()

function(set_public_includes target_name includes)
    foreach(inc_dir IN LISTS includes)
        if (NOT IS_ABSOLUTE ${inc_dir})
            set(inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}")
        endif()
        file(RELATIVE_PATH include_dir_relative_path ${PROJECT_SOURCE_DIR} ${inc_dir})
        target_include_directories(${target_name} PUBLIC
            $<BUILD_INTERFACE:${inc_dir}>
            $<INSTALL_INTERFACE:${_QSDF_HEADER_INSTALL_PATH}/${inc_dir}>)
    endforeach()
endfunction()

function(add_qsdf_depends target_name)
    cmake_parse_arguments(_arg "" "" "PUBLIC;PRIVATE;STATIC" ${ARGN})
    if (${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_qsdf_depends had unparsed arguments!")
    endif()

    set(depends "${_arg_PRIVATE}")
    set(public_depends "${_arg_PUBLIC}")

    get_target_property(target_type ${target_name} TYPE)
    if (NOT target_type STREQUAL "OBJECT_LIBRARY")
        target_link_libraries(${target_name} PRIVATE ${depends} PUBLIC ${public_depends})
    endif()
endfunction()

function(sources_group_by_explorer SRCS)
    foreach(FILE ${SRCS})
        # Get the directory of the source file
        string(CONCAT SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "/" "${FILE}")
        get_filename_component(PARENT_DIR "${SRC_PATH}" DIRECTORY)

        # Remove common directory prefix to make the group
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")

        # Group into "Source Files" and "Header Files"
        if ("${SRC_PATH}" MATCHES ".*\\.cpp")
            set(GROUP "Source Files${GROUP}")
        elseif("${SRC_PATH}" MATCHES ".*\\.h")
            set(GROUP "Header Files${GROUP}")
        endif()

        source_group("${GROUP}" FILES "${SRC_PATH}")
    endforeach()
endfunction()

function(extend_qsdf_target target_name)
    set(options)
    set(oneValueArgs SOURCES_PREFIX)
    set(multiValueArgs SOURCES INCLUDES PUBLIC_INCLUDES EXPLICIT_MOC SKIP_AUTOMOC DEPENDS PUBLIC_DEPENDS STATIC_DEPENDS DEFINES PUBLIC_DEFINES)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "extend_qsdf_target had unparsed arguments!")
    endif()

    add_qsdf_depends(${target_name} PUBLIC ${_arg_PUBLIC_DEPENDS} PRIVATE ${_arg_DEPENDS} STATIC ${_arg_STATIC_DEPENDS})

    target_include_directories(${target_name} PRIVATE ${_arg_INCLUDES})
    target_sources(${target_name} PRIVATE ${_arg_SOURCES})

    sources_group_by_explorer("${_arg_SOURCES}")

    set_public_includes(${target_name} "${_arg_PUBLIC_INCLUDES}")

    foreach(file IN LISTS _arg_EXPLICIT_MOC)
        set_explicit_moc(${target_name} "${file}")
    endforeach()

    foreach(file IN LISTS _arg_SKIP_AUTOMOC)
        set_property(SOURCE ${file} PROPERTY SKIP_AUTOMOC ON)
    endforeach()

    target_compile_definitions(${target_name}
        PRIVATE ${_arg_DEFINES}
        PUBLIC ${_arg_PUBLIC_DEFINES}
    )
endfunction()

function(find_dependent_plugins varName)
    set(_RESULT ${ARGN})
    foreach(i ${ARGN})
        get_property(_DEP TARGET ${i} PROPERTY _arg_DEPENDS)
        if (_DEP)
            find_dependent_plugins(_REC ${_DEP})
            list(APPEND _RESULT ${_REC})
        endif()
    endforeach()

    if (_RESULT)
        list(REMOVE_DUPLICATES _RESULT)
        list(SORT _RESULT)
    endif()

    set("${varName}" ${_RESULT} PARENT_SCOPE)
endfunction()

function(update_cached_list name value)
    set(_temp_list "${${name}}")
    list(APPEND _temp_list "${value}")
    set(${name} "${_temp_list}" CACHE INTERNAL "***** Internal *****")
endfunction()
