add_mips_executable(ScreenDesigner
    DEPENDS Utils ExtensionSystem
    PUBLIC_DEPENDS Qt6::Widgets
    SOURCES
        diagramscene.h diagramscene.cpp
        diagramview.h diagramview.cpp
        thumbnailview.h thumbnailview.cpp
        main.cpp
        mainwindow.h mainwindow.cpp
        thumbnailscene.h thumbnailscene.cpp
)
