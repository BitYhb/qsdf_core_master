set(proj Lua)

ExternalProject_Add(${proj}
    URL http://www.lua.org/ftp/lua-5.4.4.tar.gz
    URL_HASH SHA256=164c7849653b80ae67bec4b7473b884bf5cc8d2dca05653475ec2ed27b9ebf61)
