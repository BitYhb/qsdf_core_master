set(proj Lua)

set(${proj}_DEPENDENCIES)

ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY git@github.com:walterschell/Lua.git
    DOWNLOAD_EXTRACT_TIMESTAMP true
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
        -DLUA_BUILD_BINARY:BOOL=OFF
        -DLUA_BUILD_COMPILER:BOOL=OFF)
