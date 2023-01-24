cmake_minimum_required(VERSION 3.21)

project(lua-cmake VERSION 1.0.0 DESCRIPTION "Lua build for a project subdirectory" LANGUAGES C)

include(CheckLanguage)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Lua)

option(BUILD_SHARED_LIBS "Global flag to cause add_library() to create shared libraries if on." ON)

option(LUA_BUILD_AS_CXX "Build Lua C++" OFF)

check_language(CXX)
if(CMAKE_CXX_COMPILER)
    enable_language(CXX)
else()
    message(STATUS "No CXX support")
endif()

set(lua_source_files lapi.c lauxlib.c lbaselib.c lcode.c lcorolib.c lctype.c ldblib.c ldebug.c ldo.c
    ldump.c lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c
    loslib.c lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c)

set(lua_public_header_files_c lauxlib.h lua.h luaconf.h lualib.h)

list(TRANSFORM lua_source_files PREPEND "src/")
list(TRANSFORM lua_public_header_files_c PREPEND "src/")

add_library(lua_header INTERFACE)
target_include_directories(lua_header INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)
target_sources(lua_header PUBLIC
    FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${lua_public_header_files_c})

add_library(lua ${lua_source_files})
target_link_libraries(lua INTERFACE lua_header)

include(CMakePackageConfigHelpers)
configure_package_config_file(LuaConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LuaConfig.cmake
    INSTALL_DESTINATION "lib/cmake")

install(TARGETS lua lua_header
    EXPORT LuaTargets FILE_SET HEADERS)

install(EXPORT LuaTargets
    DESTINATION "lib/cmake"
    FILE LuaTargets.cmake
    NAMESPACE Lua::)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LuaConfig.cmake
    DESTINATION "lib/cmake")

