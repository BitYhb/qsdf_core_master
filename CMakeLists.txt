cmake_minimum_required(VERSION 3.21)

## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/super_build")

set(SUPERBUILD_TOPLEVEL_PROJECT "qsdf-core")
include(FeatureSummary)
include(GenerateExportHeader)
include(ExternalProject)
include(ExternalProjectDependency)
include(FrameworkAPI)

# Quick Software Development Framework
project(qsdf-core VERSION ${QSDF_VERSION})

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

qsdf_handle_sccache_support()

# merge binary directories of sub projects into top level
set(QSDF_MERGE_BINARY_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# SuperBuild Option - Enabled by default
option(QSDF_SUPERBUILD "Build the projects that ${PROJECT_NAME} depends on." ON)
mark_as_advanced(QSDF_SUPERBUILD)
set(QSDF_BINARY_INNER_SUBDIR ${PROJECT_NAME}-build)

option(QSDF_BUILD_SDK "Build an SDK for app or domain development" OFF)
mark_as_advanced(QSDF_BUILD_SDK)
mark_as_superbuild(QSDF_BUILD_SDK)

option(WITH_TESTS "Build Tests" OFF)
option(WITH_DEBUG_CMAKE "Enabled CMake project debugging functionality" OFF)

# Set up Qt stuff:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WITH_USE_SOLUTION_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Qt
if(NOT DEFINED QSDF_REQUIRED_QT_VERSION)
    set(_required_qt_version ${QSDF_QT_VERSION_MIN})
    set(QSDF_REQUIRED_QT_VERSION ${_required_qt_version} CACHE STRING "Minimum required Qt version" FORCE)
endif()

set(QSDF_REQUIRED_QT_MODULES
    Core Network Quick
    LinguistTools # no dll
    Widgets)

if(WITH_TESTS)
    list(APPEND QSDF_REQUIRED_QT_MODULES Test)
    set(IMPLICIT_DEPENDS Qt6::Test)
endif()

include(FindQtAndCheckVersion)
mark_as_superbuild(VARS Qt6_DIR LABELS "FIND_PACKAGE")

if(DEFINED QSDF_DOMAIN_NAME)
    mark_as_superbuild(QSDF_DOMAIN_NAME)
    message(STATUS "Using domain name: ${QSDF_DOMAIN_NAME}")
endif()

# SuperBuild script
if(QSDF_SUPERBUILD)
    include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
    return()
endif()

if(MSVC)
    option(WITH_USE_SOLUTION_FOLDERS "Use the VS solution folder" ON)
endif()
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")

# System flags
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(STATUS "The target operating system is WIN")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# Test
if(WITH_TESTS)
    enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(share)
add_subdirectory(doc)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

# install cmake module
export(EXPORT QSDFTargets
    NAMESPACE QSDF::
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/QSDFTargets.cmake)
configure_file(cmake/QSDFConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/QSDFConfig.cmake
    COPYONLY)

if(QSDF_BUILD_SDK)
    file(GLOB cmake_files "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake")
    install(
        FILES ${cmake_files}
        DESTINATION ${QSDF_CMAKE_INSTALL_PATH}/QSDF COMPONENT Development)

    install(EXPORT QSDFTargets
        DESTINATION ${QSDF_CMAKE_INSTALL_PATH}/QSDF
        COMPONENT Development
        NAMESPACE QSDF::
        FILE QSDFTargets.cmake)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/QSDFConfig.cmake
        ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPI.cmake
        ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPIInternal.cmake
        DESTINATION ${QSDF_CMAKE_INSTALL_PATH}/QSDF
        COMPONENT Development)
endif()

