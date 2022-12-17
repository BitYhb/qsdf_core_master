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

# Internal SuperBuild Option - Enabled by default
option(QSDF_INTERNAL_SUPERBUILD "Build the internal projects that ${PROJECT_NAME} depends on." ON)
mark_as_advanced(QSDF_INTERNAL_SUPERBUILD)

option(WITH_TESTS "Build Tests" OFF)
add_feature_info("Build Tests" ${WITH_TESTS} "")
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
endif()

# SuperBuild script
if(QSDF_SUPERBUILD)
    include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
    return()
endif()

# Internal SuperBuild script
if(QSDF_INTERNAL_SUPERBUILD)
    include("${CMAKE_CURRENT_SOURCE_DIR}/InternalSuperBuild.cmake")
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
