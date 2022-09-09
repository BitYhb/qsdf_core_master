set(EXTENSIONSYSTEM_PUBLIC_HEADERS
    extensionsystem_global.h
    iobserver.h
    iplugin.h
    isubject.h
    optionsparser.h
    pluginerroroverview.h
    pluginmanager.h
    pluginspec.h)

set(EXTENSIONSYSTEM_PUBLIC_SOURCES
    iobserver.cpp
    iplugin.cpp
    isubject.cpp
    optionsparser.cpp
    pluginerroroverview.cpp
    pluginmanager.cpp
    pluginspec.cpp)

### *** Internal ***
set(EXTENSIONSYSTEM_PRIVATE_HEADERS_AND_SOURCES
    pluginmanager_p.h
    pluginspec_p.h)

add_mips_library(ExtensionSystem
    DEPENDS Utils
    PUBLIC_DEPENDS Qt6::Core
    MSVC_SOLUTION_FOLDER "${CORE_SOLUTION_FOLDER}"
    SOURCES ${EXTENSIONSYSTEM_PUBLIC_HEADERS} ${EXTENSIONSYSTEM_PUBLIC_SOURCES}
    ${EXTENSIONSYSTEM_PRIVATE_HEADERS_AND_SOURCES})

#mips_copy_to_builddir(copy_extensionsystem_header_to_builddir
#    FILES ${EXTENSIONSYSTEM_PUBLIC_HEADERS}
#    DESTINATION "${MIPS_HEADER_INSTALL_PATH}/extensionsystem"
#)
