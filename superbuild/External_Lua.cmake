set(proj Lua)

set(${proj}_DEPENDENCIES)

ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    URL http://www.lua.org/ftp/lua-5.4.4.tar.gz
    URL_HASH SHA256=164c7849653b80ae67bec4b7473b884bf5cc8d2dca05653475ec2ed27b9ebf61
    DOWNLOAD_EXTRACT_TIMESTAMP true)
