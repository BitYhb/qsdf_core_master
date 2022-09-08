set(UTILS_PUBLIC_HEADERS
    singleapplication/singleapplication.h
    theme/theme.h
    algorithm.h
    benchmarker.h
    flowgraphicslayout.h
    id.h
    mimetypes.h
    mipsapplication.h
    mipsassert.h
    mipssettings.h
    stringutils.h
    stylehelper.h
    utils_global.h
    macrodefinition.h
)

set(UTILS_PUBLIC_SOURCES
    singleapplication/singleapplication.cpp
    theme/theme.cpp
    benchmarker.cpp
    flowgraphicslayout.cpp
    id.cpp
    mimetypes.cpp
    mipsapplication.cpp
    mipsassert.cpp
    mipssettings.cpp
    stringutils.cpp
    stylehelper.cpp
    macrodefinition.cpp
)

### *** Internal ***
set(UTILS_PRIVATE_HEADERS_AND_SOURCES
    singleapplication/localpeer.h singleapplication/localpeer.cpp
    singleapplication/lockedfile.h singleapplication/lockedfile.cpp
    theme/theme_p.h
)

qt_add_resources(UTILS_RESOURCES utils.qrc)

add_mips_library(Utils
    PUBLIC_DEPENDS Qt6::Core Qt6::Widgets Qt6::Network
    MSVC_SOLUTION_FOLDER "${CORE_SOLUTION_FOLDER}"
    SOURCES
        ${UTILS_PUBLIC_HEADERS}
        ${UTILS_PUBLIC_SOURCES}
        ${UTILS_PRIVATE_HEADERS_AND_SOURCES}
        ${UTILS_RESOURCES}
)

#mips_copy_to_builddir(copy_utils_header_to_builddir
#    FILES ${UTILS_PUBLIC_HEADERS}
#    DESTINATION "${MIPS_HEADER_INSTALL_PATH}/utils"
#)
