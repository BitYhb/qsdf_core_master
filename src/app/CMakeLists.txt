configure_file(app_version.h.cmakein app_version.h ESCAPE_QUOTES)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/app_version.h
    DESTINATION ${QSDF_HEADER_INSTALL_PATH}/src/app
    COMPONENT Devel EXCLUDE_FROM_ALL
)

add_qsdf_executable(launch
    DEPENDS app_version Utils ExtensionSystem
    PUBLIC_DEPENDS Qt6::Widgets
    MSVC_SOLUTION_FOLDER "${APP_SOLUTION_FOLDER}"
    SOURCES
        main.cpp
)
