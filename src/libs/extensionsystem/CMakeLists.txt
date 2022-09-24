set(PUBLIC_HEADERS
    iplugin.h
    optionsparser.h
    pluginmanager.h
    pluginspec.h)

set(PUBLIC_SOURCES
    iplugin.cpp
    optionsparser.cpp
    pluginmanager.cpp
    pluginspec.cpp)

### *** Internal ***
set(PRIVATE_HEADERS
    pluginmanager_p.h
    pluginspec_p.h)

add_mips_library(ExtensionSystem
    DEPENDS Utils
    PUBLIC_DEPENDS Qt6::Core
    MSVC_SOLUTION_FOLDER "${CORE_SOLUTION_FOLDER}"
    SOURCES ${PUBLIC_HEADERS} ${PUBLIC_SOURCES} ${PRIVATE_HEADERS})

#mips_copy_to_builddir(copy_extensionsystem_header_to_builddir
#    FILES ${EXTENSIONSYSTEM_PUBLIC_HEADERS}
#    DESTINATION "${MIPS_HEADER_INSTALL_PATH}/extensionsystem"
#)
