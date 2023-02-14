set(QSDF_QT_VERSION_MIN "6.2.0")

include(${CMAKE_CURRENT_LIST_DIR}/FrameworkAPIInternal.cmake)

set(QSDF_HEADER_INSTALL_PATH ${_QSDF_HEADER_INSTALL_PATH})
set(QSDF_CMAKE_INSTALL_PATH ${_QSDF_CMAKE_INSTALL_PATH})

if(ANDROID)
    set(QSDF_ANDROID_PACKAGE_ROOT "android")
    set(QSDF_ANDROID_LIB_INSTALL_PATH "${QSDF_ANDROID_PACKAGE_ROOT}/libs/${ANDROID_ABI}")
endif()

set(QSDF_DATA_PATH ${_QSDF_DATA_PATH})
set(QSDF_PLUGIN_PATH "${_QSDF_PLUGIN_PATH}")

set(APP_SOLUTION_FOLDER "${_APP_SOLUTION_FOLDER}")
set(PLUGINS_SOLUTION_FOLDER "${_PLUGINS_SOLUTION_FOLDER}")
set(CORE_SOLUTION_FOLDER "${_CORE_SOLUTION_FOLDER}")
set(SHARED_SOLUTION_FOLDER "${_SHARED_SOLUTION_FOLDER}")
set(TOOLS_SOLUTION_FOLDER "${_TOOLS_SOLUTION_FOLDER}")
set(OTHER_SOLUTION_FOLDER "${_OTHER_SOLUTION_FOLDER}")

cmake_path(RELATIVE_PATH QSDF_DATA_PATH BASE_DIRECTORY "${CMAKE_INSTALL_PREFIX}" OUTPUT_VARIABLE RELATIVE_DATA_PATH)

set(RELATIVE_PLUGIN_PATH ${QSDF_PLUGIN_PATH})
if(WIN32)
    cmake_path(RELATIVE_PATH QSDF_PLUGIN_PATH BASE_DIRECTORY "${CMAKE_INSTALL_PREFIX}" OUTPUT_VARIABLE RELATIVE_PLUGIN_PATH)
endif()

list(APPEND DEFAULT_DEFINES
    RELATIVE_DATA_PATH="${RELATIVE_DATA_PATH}"
    RELATIVE_PLUGIN_PATH="${RELATIVE_PLUGIN_PATH}")

option(BUILD_TESTS_BY_DEFAULT "Build tests by default. This can be used to build all tests by default, or none." ON)
option(WITH_SCCACHE_SUPPORT "Enables support for building with SCCACHE and separate debug info with MSVC, which SCCACHE normally doesn't support." OFF)
option(QSDF_STATIC_BUILD "Builds libraries and plugins as static libraries" OFF)

function(reset_msvc_output_path target_name)
    foreach(config DEBUG RELEASE)
        foreach(flag RUNTIME LIBRARY ARCHIVE)
            get_target_property(target_property ${target_name} "${flag}_OUTPUT_DIRECTORY")
            if(NOT target_property STREQUAL "" AND NOT STREQUAL "target_property-NOTFOUND")
                set_target_properties(${target_name} PROPERTIES "${flag}_OUTPUT_DIRECTORY_${config}" ${target_property})
            endif()
        endforeach()
    endforeach()
endfunction()

function(qsdf_source_dir varName)
    if(QSDF_MERGE_BINARY_DIR)
        set(${varName} ${QSDF_SOURCE_DIR} PARENT_SCOPE)
    else()
        set(${varName} ${PROJECT_SOURCE_DIR} PARENT_SCOPE)
    endif()
endfunction()

function(qsdf_delay_add_compile_definitions_to_plugin_core)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(_arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments: ${_arg_UNPARSED_ARGUMENTS}")
    endif()

    set_property(GLOBAL APPEND PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PUBLIC ${_arg_PUBLIC})
    set_property(GLOBAL APPEND PROPERTY Core_DELAY_ADD_COMPILE_DEFINITIONS_PRIVATE ${_arg_PRIVATE})
endfunction()

function(qsdf_copy_to_builddir custom_target_name)
    cmake_parse_arguments(_arg "CREATE_SUBDIRS" "DESTINATION" "FILES;DIRECTORIES" ${ARGN})
    set(timestampFiles)

    set(allFiles ${_arg_FILES})

    # FILES
    foreach(srcFile ${_arg_FILES})
        string(MAKE_C_IDENTIFIER "${srcFile}" destinationTimestampFilePart)
        set(destinationTimestampFileName "${CMAKE_CURRENT_BINARY_DIR}/.${destinationTimestampFilePart}_timestamp")
        list(APPEND timestampFiles "${destinationTimestampFileName}")

        if(IS_ABSOLUTE "${srcFile}")
            set(srcPath "")
        else()
            get_filename_component(srcPath "${srcFile}" DIRECTORY)
        endif()

        add_custom_command(OUTPUT "${destinationTimestampFileName}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${_arg_DESTINATION}/${srcPath}"
            COMMAND ${CMAKE_COMMAND} -E copy "${srcFile}" "${CMAKE_BINARY_DIR}/${_arg_DESTINATION}/${srcPath}"
            COMMAND ${CMAKE_COMMAND} -E touch "${destinationTimestampFileName}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Copy ${srcFile} into build directory"
            DEPENDS "${srcFile}")
    endforeach()

    # DIRECTORIES
    foreach(srcDirectory ${_arg_DIRECTORIES})
        string(MAKE_C_IDENTIFIER "${srcDirectory}" destinationTimestampFilePart)
        set(destinationTimestampFileName "${CMAKE_CURRENT_BINARY_DIR}/.${destinationTimestampFilePart}_timestamp")
        list(APPEND timestampFiles "${destinationTimestampFileName}")
        set(destinationDirectory "${CMAKE_BINARY_DIR}/${_arg_DESTINATION}")

        if(_arg_CREATE_SUBDIRS)
            set(destinationDirectory "${destinationDirectory}/${srcDirectory}")
        endif()

        file(GLOB_RECURSE filesToCopy "${srcDirectory}/*")
        list(APPEND allFiles ${filesToCopy})
        add_custom_command(OUTPUT "${destinationTimestampFileName}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${srcDirectory}" "${destinationDirectory}"
            COMMAND "${CMAKE_COMMAND}" -E touch "${destinationTimestampFileName}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Copy ${srcDirectory}/ into build directory"
            DEPENDS ${filesToCopy}
            VERBATIM)
    endforeach()

    add_custom_target("${custom_target_name}" ALL DEPENDS ${timestampFiles}
        SOURCES ${allFiles})
endfunction()

function(add_qsdf_library target_name)
    set(options STATIC OBJECT QML_PLUGIN)
    set(oneValueArgs DESTINATION SOURCES_PREFIX MSVC_SOLUTION_FOLDER)
    set(multiValueArgs SOURCES INCLUDES PUBLIC_INCLUDES EXPLICIT_MOC SKIP_AUTOMOC DEPENDS PUBLIC_DEPENDS
        STATIC_DEPENDS DEFINES PUBLIC_DEFINES PROPERTIES)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_qsdf_library had unparsed arguments!")
    endif()

    set(library_type SHARED)
    if(_arg_STATIC)
        set(library_type STATIC)
    endif()

    add_library(${target_name} ${library_type})
    add_library(QSDF::${target_name} ALIAS ${target_name})

    if(ANDROID)
        qt_android_apply_arch_suffix(${target_name})
    endif()

    set(_DESTINATION ${CMAKE_INSTALL_PREFIX})
    if(_arg_DESTINATION)
        set(_DESTINATION ${_arg_DESTINATION})
    endif()

    set(_MSVC_SOLUTION_FOLDER "")
    if(_arg_MSVC_SOLUTION_FOLDER)
        set(_MSVC_SOLUTION_FOLDER "${_arg_MSVC_SOLUTION_FOLDER}")
    endif()

    # export header file
    foreach(source ${_arg_SOURCES})
        if(${source} MATCHES "^.*\.h$" AND NOT ${source} MATCHES "^.*_p\.h$")
            list(APPEND _public_headers ${source})
        endif()
    endforeach()
    generate_export_header(${target_name} PREFIX_NAME QSDF_)
    string(TOLOWER ${target_name} lower_target_name)
    target_sources(${target_name} PUBLIC
        FILE_SET export_headers
            TYPE HEADERS
            BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${lower_target_name}_export.h
        FILE_SET public_headers
            TYPE HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES ${_public_headers})

    extend_qsdf_target(${target_name}
        SOURCES ${_arg_SOURCES}
        INCLUDES ${_arg_INCLUDES}
        PUBLIC_INCLUDES ${_arg_PUBLIC_INCLUDES}
        SOURCES_PREFIX ${_arg_SOURCES_PREFIX}
        EXPLICIT_MOC ${_arg_EXPLICIT_MOC}
        SKIP_AUTOMOC ${_arg_SKIP_AUTOMOC}
        DEFINES ${_arg_DEFINES} ${DEFAULT_DEFINES}
        PUBLIC_DEFINES ${_arg_PUBLIC_DEFINES}
        STATIC_DEPENDS ${_arg_STATIC_DEPENDS}
        DEPENDS ${_arg_DEPENDS}
        PUBLIC_DEPENDS ${_arg_PUBLIC_DEPENDS})

    # everything is different with SOURCES_PREFIX
    include(GNUInstallDirs)
    if(NOT _arg_SOURCES_PREFIX)
        get_filename_component(public_build_interface_dir "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
        file(RELATIVE_PATH include_dir_relative_path ${PROJECT_SOURCE_DIR} "${public_build_interface_dir}")

        set(_true_string $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${lower_target_name}>)
        set(_false_string $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
        target_include_directories(${target_name}
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
            PUBLIC
                "$<BUILD_INTERFACE:${public_build_interface_dir}>"
                "$<INSTALL_INTERFACE:$<IF:$<BOOL:${QSDF_BUILD_RELEASE_PACKAGE}>,${_true_string},${_false_string}>>")
        unset(_true_string)
        unset(_false_string)
    endif()

    string(REGEX MATCH "^[0-9]*" QSDF_VERSION_MAJOR ${QSDF_VERSION})
    set_target_properties(${target_name} PROPERTIES
        LINK_DEPENDS_NO_SHARED ON
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        VERSION ${QSDF_VERSION}
        SOVERSION ${QSDF_VERSION_MAJOR}
        CXX_EXTENSIONS OFF
        FOLDER "${_MSVC_SOLUTION_FOLDER}"
        BUILD_RPATH "${_LIB_RPATH};${CMAKE_BUILD_RPATH}"
        INSTALL_RPATH "${_LIB_RPATH};${CMAKE_INSTALL_RPATH}"
        ${_arg_PROPERTIES})

    reset_msvc_output_path(${target_name})

    include(GNUInstallDirs)
    if(QSDF_BUILD_SDK)
        install(TARGETS ${target_name} EXPORT QSDFTargets
            FILE_SET export_headers DESTINATION sdk/${CMAKE_INSTALL_INCLUDEDIR}/${lower_target_name} COMPONENT DevelopmentKit
            FILE_SET public_headers DESTINATION sdk/${CMAKE_INSTALL_INCLUDEDIR}/${lower_target_name} COMPONENT DevelopmentKit
            RUNTIME DESTINATION sdk/${CMAKE_INSTALL_BINDIR} COMPONENT DevelopmentKit
            LIBRARY DESTINATION sdk/${CMAKE_INSTALL_LIBDIR} COMPONENT DevelopmentKit
            ARCHIVE DESTINATION sdk/${CMAKE_INSTALL_LIBDIR} COMPONENT DevelopmentKit)
    endif()

    if(QSDF_BUILD_RELEASE_PACKAGE)
        install(TARGETS ${target_name}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ReleasePackage
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ReleasePackage
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ReleasePackage)
    endif()

    if(ANDROID)
        install(TARGETS ${target_name} DESTINATION ${QSDF_ANDROID_LIB_INSTALL_PATH})
    endif()
endfunction()

function(add_qsdf_plugin target_name)
    set(options)
    set(oneValueArgs PLUGIN_PATH MSVC_SOLUTION_FOLDER)
    set(multiValueArgs SOURCES INCLUDES PUBLIC_INCLUDES EXPLICIT_MOC SKIP_AUTOMOC DEPENDS PUBLIC_DEPENDS
        DEFINES PUBLIC_DEFINES PLUGIN_DEPENDS)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_qsdf_plugin had unparsed arguments!")
    endif()

    set(_MSVC_SOLUTION_FOLDER "")
    if(_arg_MSVC_SOLUTION_FOLDER)
        set(_MSVC_SOLUTION_FOLDER "${_arg_MSVC_SOLUTION_FOLDER}")
    endif()

    ### Generate dependency list:
    if(NOT _arg_VERSION)
        set(_arg_VERSION ${QSDF_VERSION})
    endif()
    find_dependent_plugins(_PLUGIN_DEPENDS ${_arg_PLUGIN_DEPENDS})
    set(_arg_DEPENDENCY_STRING "\"Dependencies\": [\n")
    foreach(_plugin_i IN LISTS _PLUGIN_DEPENDS)
        if(_plugin_i MATCHES "^QSDF::")
            set(_VERSION ${QSDF_VERSION})
            string(REPLACE "QSDF::" "" _plugin_i ${_plugin_i})
        else()
            get_property(_VERSION TARGET ${_plugin_i} PROPERTY _arg_VERSION)
        endif()

        string(APPEND _arg_DEPENDENCY_STRING "{\"Name\":\"${_plugin_i}\", \"Version\": \"${_VERSION}\"}")
    endforeach()
    string(APPEND _arg_DEPENDENCY_STRING "\n    ]")

    set(QSDF_PLUGIN_DEPENDENCY_STRING ${_arg_DEPENDENCY_STRING})

    ### Configure plugin.json file:
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target_name}.json.in")
        file(READ "${target_name}.json.in" plugin_json_in)
        string(REPLACE "\\\"" "\"" plugin_json_in ${plugin_json_in})
        string(REPLACE "$$QSDF_VERSION" "\${QSDF_VERSION}" plugin_json_in ${plugin_json_in})
        string(REPLACE "$$dependencyList" "\${QSDF_PLUGIN_DEPENDENCY_STRING}" plugin_json_in ${plugin_json_in})
        string(CONFIGURE "${plugin_json_in}" plugin_json)
        file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.json" CONTENT "${plugin_json}")
    endif()

    add_library(${target_name} SHARED ${_arg_SOURCES})
    add_library(QSDF::${target_name} ALIAS ${target_name})

    if(ANDROID)
        qt_android_apply_arch_suffix(${target_name})
    endif()

    set_public_headers(${target_name} ${_arg_SOURCES})

    generate_export_header(${target_name} PREFIX_NAME QSDF_PLUGIN_)
    string(TOLOWER ${target_name} lower_target_name)
    target_sources(${target_name} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${lower_target_name}_export.h")

    extend_qsdf_target(${target_name}
        SOURCES ${_arg_SOURCES}
        INCLUDES ${_arg_INCLUDES}
        PUBLIC_INCLUDES ${_arg_PUBLIC_INCLUDES}
        EXPLICIT_MOC ${_arg_EXPLICIT_MOC}
        SKIP_AUTOMOC ${_arg_SKIP_AUTOMOC}
        DEFINES ${_arg_DEFINES} ${DEFAULT_DEFINES}
        PUBLIC_DEFINES ${_arg_PUBLIC_DEFINES}
        DEPENDS ${_arg_DEPENDS}
        PUBLIC_DEPENDS ${_arg_PUBLIC_DEPENDS})

    get_filename_component(public_build_interface_dir "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    file(RELATIVE_PATH include_dir_relative_path ${PROJECT_SOURCE_DIR} "${public_build_interface_dir}")
    target_include_directories(${target_name}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${CMAKE_BINARY_DIR}/src"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        PUBLIC
            "$<BUILD_INTERFACE:${public_build_interface_dir}>"
            "$<INSTALL_INTERFACE:${QSDF_HEADER_INSTALL_PATH}/${include_dir_relative_path}>")

    set(plugin_dir "${QSDF_PLUGIN_PATH}")
    if(_arg_PLUGIN_PATH)
        set(plugin_dir "${_arg_PLUGIN_PATH}")
    endif()

    set_target_properties(${target_name} PROPERTIES
        LINK_DEPENDS_NO_SHARED ON
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        CXX_EXTENSIONS OFF
        BUILD_RPATH "${_PLUGIN_RPATH};${CMAKE_BUILD_RPATH}"
        INSTALL_RPATH "${_PLUGIN_RPATH};${CMAKE_INSTALL_RPATH}"
        FOLDER "${_MSVC_SOLUTION_FOLDER}"
        _arg_DEPENDS "${_arg_PUBLIC_DEPENDS}"
        _arg_VERSION "${_arg_VERSION}"
        ${_arg_PROPERTIES})

    reset_msvc_output_path(${target_name})

    include(GNUInstallDirs)
    if(QSDF_BUILD_RELEASE_PACKAGE)
        install(TARGETS ${target_name}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ReleasePackage
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ReleasePackage
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ReleasePackage)
    endif()

    if(ANDROID)
        install(TARGETS ${target_name} DESTINATION ${QSDF_ANDROID_LIB_INSTALL_PATH})
    endif()
endfunction()

function(add_qsdf_executable target_name)
    set(options)
    set(oneValueArgs DESTINATION MSVC_SOLUTION_FOLDER)
    set(multiValueArgs SOURCES INCLUDES DEPENDS PUBLIC_DEPENDS DEFINES PUBLIC_DEFINES PROPERTIES)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_qsdf_executable had unparsed arguments!")
    endif()

    set(_MSVC_SOLUTION_FOLDER "")
    if(_arg_MSVC_SOLUTION_FOLDER)
        set(_MSVC_SOLUTION_FOLDER "${_arg_MSVC_SOLUTION_FOLDER}")
    endif()

    if(ANDROID)
        add_library(${target_name} MODULE ${_arg_SOURCES})
        # On our qmake builds we do don't compile the executables with
        # visibility=hidden. Not having this flag set will cause the
        # executable to have main() hidden and can then no longer be loaded
        # through dlopen()
        set_property(TARGET ${target_name} PROPERTY C_VISIBILITY_PRESET default)
        set_property(TARGET ${target_name} PROPERTY CXX_VISIBILITY_PRESET default)
        set_property(TARGET ${target_name} PROPERTY OBJC_VISIBILITY_PRESET default)
        set_property(TARGET ${target_name} PROPERTY OBJCXX_VISIBILITY_PRESET default)
        qt_android_apply_arch_suffix(${target_name})
        set_property(TARGET ${target_name} PROPERTY _qt_is_android_executable True)
    else()
        qt_add_executable(${target_name} ${_arg_SOURCES})
    endif()

    extend_qsdf_target(${target_name}
        INCLUDES "${CMAKE_BINARY_DIR}/src" ${_arg_INCLUDES}
        DEFINES ${_arg_DEFINES} ${DEFAULT_DEFINES}
        PUBLIC_DEFINES ${_arg_PUBLIC_DEFINES}
        DEPENDS ${_arg_DEPENDS}
        PUBLIC_DEPENDS ${_arg_PUBLIC_DEPENDS})

    set(_DESTINATION ${CMAKE_INSTALL_PREFIX})
    if(_arg_DESTINATION)
        set(_DESTINATION ${_arg_DESTINATION})
    endif()

    set(_EXECUTABLE_PATH ${_DESTINATION})
    file(RELATIVE_PATH relative_lib_path "/${_EXECUTABLE_PATH}" "/${CMAKE_INSTALL_PREFIX}")

    set(build_rpath "${_RPATH_BASE}/${relative_lib_path};${CMAKE_BUILD_RPATH}")
    set(install_rpath "${_RPATH_BASE}/${relative_lib_path};${CMAKE_INSTALL_RPATH}")

    set_target_properties(${target_name} PROPERTIES
        LINK_DEPENDS_NO_SHARED ON
        BUILD_RPATH "${build_rpath}"
        INSTALL_RPATH "${install_rpath}"
        CXX_EXTENSIONS OFF
        FOLDER "${_MSVC_SOLUTION_FOLDER}"
        WIN32_EXECUTABLE TRUE
        ${_arg_PROPERTIES})

    reset_msvc_output_path(${target_name})

    if(ANDROID)
        install(TARGETS ${target_name}
            LIBRARY DESTINATION ${QSDF_ANDROID_LIB_INSTALL_PATH})
    endif()
endfunction()

function(add_qsdf_test target_name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES INCLUDES DEPENDS DEFINES CONDITION EXPLICIT_MOC SKIP_AUTOMOC)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_qsdf_test had unparsed arguments!")
    endif()

    update_cached_list(__QSDF_TESTS ${target_name})

    if(NOT _arg_CONDITION)
        set(_arg_CONDITION ON)
    endif()

    string(TOUPPER "BUILD_TEST_${target_name}" _build_test_var)
    set(_build_test_by_default ${BUILD_TESTS_BY_DEFAULT})
    if(DEFINED ENV{QSDF_${_build_test_var}})
        set(_build_test_by_default "$ENV{QSDF_${_build_test_var}}")
    endif()
    set(${_build_test_var} "${_build_test_by_default}" CACHE BOOL "Build test ${target_name}")

    if(NOT ${_build_test_var} OR NOT ${_arg_CONDITION})
        return()
    endif()

    foreach(dependency ${_arg_DEPENDS})
        if(NOT TARGET ${dependency} AND NOT _arg_GTEST)
            if(WITH_DEBUG_CMAKE)
                message(STATUS "'${dependency}' is not a target")
            endif()
            return()
        endif()
    endforeach()

    set(TEST_DEFINES SRCDIR=${CMAKE_CURRENT_SOURCE_DIR})

    # relax cast requirements for tests
    set(default_defines_copy ${DEFAULT_DEFINES})
    list(REMOVE_ITEM default_defines_copy QT_NO_CAST_TO_ASCII QT_RESTRICTED_CAST_FROM_ASCII)

    file(RELATIVE_PATH _RPATH "/${CMAKE_INSTALL_PREFIX}" "/${CMAKE_INSTALL_PREFIX}")

    add_executable(${target_name} ${_arg_SOURCES})

    extend_qsdf_target(${target_name}
        DEPENDS ${_arg_DEPENDS} ${IMPLICIT_DEPENDS}
        INCLUDES "${CMAKE_BINARY_DIR}/src" ${_arg_INCLUDES}
        DEFINES ${_arg_DEFINES} ${TEST_DEFINES} ${default_defines_copy}
        EXPLICIT_MOC ${_arg_EXPLICIT_MOC}
        SKIP_AUTOMOC ${_arg_SKIP_AUTOMOC})

    set_target_properties(${target_name} PROPERTIES
        LINK_DEPENDS_NO_SHARED ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        BUILD_RPATH "${_RPATH_BASE}/${_RPATH};${CMAKE_BUILD_RPATH}"
        INSTALL_RPATH "${_RPATH_BASE}/${_RPATH};${CMAKE_INSTALL_RPATH}")

    reset_msvc_output_path(${target_name})
endfunction()

function(add_translation_targets target_name)
    set(options)
    set(oneValueArgs OUTPUT_DIRECTORY)
    set(multiValueArgs LANGUAGES)
    cmake_parse_arguments(_arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${_arg_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "add_translation_targets had unparsed arguments!")
    endif()

    set(language_files)
    foreach(language ${_arg_LANGUAGES})
        string(TOLOWER ${target_name} lower_target_name)
        string(CONCAT language_file "${lower_target_name}_" "${language}" ".ts")
        list(APPEND language_files ${language_file})
    endforeach()

    set_source_files_properties(${language_files} PROPERTIES OUTPUT_LOCATION "${OUTPUT_DIRECTORY}")

    qt_add_translations(${target_name} TS_FILES ${language_files}
        QM_FILES_OUTPUT_VARIABLE qm_files)
endfunction()
