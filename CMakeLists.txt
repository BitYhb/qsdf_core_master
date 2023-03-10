cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

## Default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/super_build")

set(SUPERBUILD_TOPLEVEL_PROJECT "qsdf-core")

include(FeatureSummary)
include(GenerateExportHeader)
include(ExternalProject)
include(ExternalProjectDependency)

# vcpkg
option(QSDF_VCPKG_AUTO_INSTALL "Enable vcpkg as a package management tool." OFF)
if(QSDF_VCPKG_AUTO_INSTALL)
    include(AutoVcpkg)
endif()

# Android
option(QSDF_BUILD_ANDROID "The software will be compiled into Android program" OFF)
if(QSDF_BUILD_ANDROID)
    include(AutoAndroid)
endif()
mark_as_superbuild(QSDF_BUILD_ANDROID)

# Quick Software Development Framework
project(qsdf-core LANGUAGES CXX)

include(FrameworkAPI)

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

# Because the SDK Path will be used when compiling EPDomain_*, it is enabled by default.
option(QSDF_BUILD_SDK "Build an SDK for app or domain development" ON)
if(QSDF_BUILD_SDK AND NOT DEFINED QSDF_INSTALL_ROOT)
    set(QSDF_INSTALL_ROOT ${CMAKE_CURRENT_BINARY_DIR})
endif()
mark_as_advanced(QSDF_BUILD_SDK)
mark_as_superbuild(QSDF_BUILD_SDK)

option(QSDF_BUILD_APP_BUNDLE "Build application package" OFF)
if(QSDF_BUILD_APP_BUNDLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "QSDF_BUILD_APP_BUNDLE requires a build type of Release")
    endif()

    set(QSDF_BUILD_RELEASE_PACKAGE ON)
    set(CPACK_INSTALL_CMAKE_PROJECTS)
    mark_as_superbuild(VARS QSDF_BUILD_RELEASE_PACKAGE:BOOL ALL_PROJECTS)
    message(STATUS "QSDF_BUILD_APP_BUNDLE = ${QSDF_BUILD_APP_BUNDLE}\n"
        "  Forcing QSDF_BUILD_RELEASE_PACKAGE = ${QSDF_BUILD_RELEASE_PACKAGE}\n"
        "  Forcing CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
endif()

set(QSDF_BUILD_APP_BUNDLE_GENERATOR IFW CACHE STRING "A generator that generates application bundles" FORCE)
set_property(CACHE QSDF_BUILD_APP_BUNDLE_GENERATOR PROPERTY STRINGS IFW NSIS)
mark_as_superbuild(QSDF_BUILD_APP_BUNDLE_GENERATOR)

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
mark_as_superbuild(VARS Qt6_DIR LABELS "FIND_PACKAGE" ALL_DOMAIN_PROJECTS)

if(DEFINED QSDF_DOMAIN_NAME)
    mark_as_superbuild(QSDF_DOMAIN_NAME)
    message(STATUS "Using domain name: ${QSDF_DOMAIN_NAME}")
endif()

include(FrameworkUtils)
check_git_branch_name()
message(STATUS "Project current git branch name: ${PROJECT_GIT_BRANCH_NAME}")

find_package(Npm)
if(NOT Npm_FOUND)
    message(FATAL_ERROR "error: Npm not found, install Npm and try to re-configure")
endif()
mark_as_superbuild(Npm_EXECUTABLE)

# SuperBuild script
if(QSDF_SUPERBUILD)
    include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")

    # CPack - bundle install
    if(QSDF_BUILD_RELEASE_PACKAGE)
        if(NOT DEFINED CPACK_IFW_ROOT AND NOT DEFINED CACHE{CPACK_IFW_ROOT})
            message(FATAL_ERROR "You probably need to set the CPACK_IFW_ROOT variable.")
        endif()
        set(CPACK_GENERATOR ${QSDF_BUILD_APP_BUNDLE_GENERATOR})

        configure_file(${CMAKE_SOURCE_DIR}/cmake/BundleInstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall.cmake @ONLY)
        install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall.cmake COMPONENT ReleasePackage)

        set(CPACK_IFW_PACKAGE_TITLE "Install")
        set(CPACK_IFW_PACKAGE_NAME "Ex")

        include(CPack)
        include(CPackIFW)
        cpack_add_component(ReleasePackage
            DISPLAY_NAME core
            DESCRIPTION "Software subject"
            REQUIRED)
        cpack_ifw_configure_component(qsdf
            FORCED_INSTALLATION REQUIRES_ADMIN_RIGHTS)
    endif()

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

add_subdirectory(src)
add_subdirectory(share)
add_subdirectory(doc)

if(WITH_TESTS)
    enable_testing()
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
        DESTINATION sdk/${QSDF_CMAKE_INSTALL_PATH}/QSDF COMPONENT DevelopmentKit)

    install(EXPORT QSDFTargets
        DESTINATION sdk/${QSDF_CMAKE_INSTALL_PATH}/QSDF
        COMPONENT DevelopmentKit
        NAMESPACE QSDF::
        FILE QSDFTargets.cmake)
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/cmake/QSDFConfig.cmake
            ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPI.cmake
            ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPIInternal.cmake
        DESTINATION sdk/${QSDF_CMAKE_INSTALL_PATH}/QSDF
        COMPONENT DevelopmentKit)
endif()