set(CORE_PUBLIC_HEADERS
    core_global.h
    coreconstants.h
    coreplugin.h
    event/mipscoreevent.h
    event/mipsevent.h
    icontext.h
    icore.h
    singlesubject.h
)

set(CORE_PUBLIC_SOURCES
    coreplugin.cpp
    event/mipscoreevent.cpp
    event/mipsevent.cpp
    icontext.cpp
    icore.cpp
    singlesubject.cpp
)

### *** Internal ***
set(CORE_PRIVATE_HEADERS_AND_SOURCES
    coreproxystyle.h coreproxystyle.cpp
    mainwindow.h mainwindow.cpp
    themechooser.h themechooser.cpp
)

qt_add_resources(CORE_RESOURCES qml.qrc)

add_mips_plugin(Core
    DEPENDS Qt6::Qml
    PUBLIC_DEPENDS ExtensionSystem Utils
    MSVC_SOLUTION_FOLDER "${PLUGINS_SOLUTION_FOLDER}"
    SOURCES
        ${CORE_PUBLIC_HEADERS}
        ${CORE_PUBLIC_SOURCES}
        ${CORE_PRIVATE_HEADERS_AND_SOURCES}
        ${CORE_RESOURCES}
)

#mips_copy_to_builddir(copy_core_header_to_builddir
#    FILES ${CORE_PUBLIC_HEADERS}
#    DESTINATION "${MIPS_HEADER_INSTALL_PATH}/coreplugin"
#)
