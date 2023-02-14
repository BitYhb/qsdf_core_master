# JDK
if(NOT DEFINED CACHE{JAVA_HOME} AND NOT DEFINED JAVA_HOME)
    set(JAVA_HOME $ENV{JAVA_HOME})
    if(NOT DEFINED JAVA_HOME)
        message(FATAL_ERROR "Could not find the JDK. Please set either the JAVA_HOME environment or CMake variable.")
    endif()
endif()
string(REPLACE "\\" "/" JAVA_HOME ${JAVA_HOME})
message(STATUS "Found JDK: ${JAVA_HOME}")
mark_as_superbuild(JAVA_HOME)

if(NOT DEFINED ANDROID_SDK_ROOT OR ANDROID_SDK_ROOT STREQUAL "")
    message(FATAL_ERROR "Could not find the Android SDK. Please set the CMake variable ANDROID_SDK_ROOT.")
endif()
string(REPLACE "\\" "/" ANDROID_SDK_ROOT ${ANDROID_SDK_ROOT})
message(STATUS "Android SDK: ${ANDROID_SDK_ROOT}")
mark_as_superbuild(VARS ANDROID_SDK_ROOT:PATH ALL_PROJECTS)

# Android NDK
if(NOT DEFINED ANDROID_NDK OR ANDROID_NDK STREQUAL "")
    message(FATAL_ERROR "Could not find the Android NDK. Please set the CMake variable ANDROID_NDK.")
endif()
string(REPLACE "\\" "/" ANDROID_NDK ${ANDROID_NDK})
message(STATUS "Android NKD: ${ANDROID_NDK}")
mark_as_superbuild(VARS ANDROID_NDK:PATH ALL_PROJECTS)

## set android platform settings
if(NOT DEFINED ANDROID_ABI)
    set(ANDROID_ABI arm64-v8a CACHE STRING "Android ABI" FORCE)
    set_property(CACHE ANDROID_ABI PROPERTY STRINGS arm64-v8a armeabi-v7a x86_64 x86)
endif()
mark_as_superbuild(VARS ANDROID_ABI:STRING ALL_PROJECTS)

set(ANDROID_NATIVE_API_LEVEL 25)
mark_as_superbuild(VARS ANDROID_NATIVE_API_LEVEL:STRING ALL_PROJECTS)

set(ANDROID_STL "c++_shared")
mark_as_superbuild(VARS ANDROID_STL:STRING ALL_PROJECTS)

set(ANDROID_TOOLCHAIN clang)
mark_as_superbuild(VARS ANDROID_TOOLCHAIN:STRING ALL_PROJECTS)

macro(__FindAndroidDevelopmentKitAndConfigure)
    # find the android ndk
    set(_android_ndk ${ANDROID_NDK})
    cmake_path(APPEND _android_ndk "build/cmake/android.toolchain.cmake" OUTPUT_VARIABLE _android_toolchain_path)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${_android_toolchain_path})
        message(STATUS "Change toolchain: ${CMAKE_TOOLCHAIN_FILE}")
    else()
        if(vcpkg_FOUND)
            set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${_android_toolchain_path})
            message(STATUS "Change vcpkg chainload toolchain: ${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}")
        endif()
    endif()
    unset(_android_ndk)

    mark_as_superbuild(VARS CMAKE_TOOLCHAIN_FILE:FILEPATH ALL_PROJECTS)
    mark_as_superbuild(VARS VCPKG_CHAINLOAD_TOOLCHAIN_FILE:FILEPATH ALL_PROJECTS)
endmacro()

__FindAndroidDevelopmentKitAndConfigure()