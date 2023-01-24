set(CORE_PUBLIC_HEADERS
    coreconstants.h
    coreplugin.h
    icontext.h
    icore.h)

set(CORE_PUBLIC_SOURCES
    coreplugin.cpp
    icontext.cpp
    icore.cpp)

### *** Internal ***
set(CORE_PRIVATE_HEADERS_AND_SOURCES
    qmlapplicationengine.h qmlapplicationengine.cpp
    corepluginmacro.h corepluginmacro.cpp
    themechooser.h themechooser.cpp
    corepluginmacro.h corepluginmacro.cpp
    framelessquickwindow.h framelessquickwindow.cpp)

qt_add_resources(CORE_RESOURCES qml.qrc)

add_qsdf_plugin(Core
    DEPENDS Qt6::Qml Qt6::Quick
    PUBLIC_DEPENDS ExtensionSystem Utils
    MSVC_SOLUTION_FOLDER "${PLUGINS_SOLUTION_FOLDER}"
    SOURCES
        ${CORE_PUBLIC_HEADERS}
        ${CORE_PUBLIC_SOURCES}
        ${CORE_PRIVATE_HEADERS_AND_SOURCES}
        ${CORE_RESOURCES})

extend_qsdf_target(Core
    DEFINES ${Core_DELAY_ADD_COMPILE_DEFINITIONS_PRIVATE}
    PUBLIC_DEFINES ${Core_DELAY_ADD_COMPILE_DEFINITIONS_PUBLIC})

#qsdf_copy_to_builddir(copy_core_header_to_builddir
#    FILES ${CORE_PUBLIC_HEADERS}
#    DESTINATION "${QSDF_HEADER_INSTALL_PATH}/coreplugin"
#)
