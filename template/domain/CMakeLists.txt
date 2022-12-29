cmake_minimum_required(VERSION 3.16)

project(EPDomain_okr VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

if(NOT DEFINED QSDF_SDK_PATH)
    message(FATAL_ERROR "error: The variable \"QSDF_SDK_PATH\" is required!")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${QSDF_SDK_PATH}")

find_package(QSDF COMPONENTS Utils REQUIRED)

qt_add_library(${PROJECT_NAME} SHARED)
qt_add_qml_module(${PROJECT_NAME}
    URI EPDomain_okr
    VERSION 1.0
    QML_FILES src/qml/Viewer.qml)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Quick QSDF::Utils)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(${PROJECT_NAME}_ENABLE_TESTS "Enabled for testing." OFF)
if(${${PROJECT_NAME}_ENABLE_TESTS})
    add_subdirectory(tests)

    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS ${PROJECT_NAME}plugin
        DESTINATION "tests/${PROJECT_NAME}")
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.qmltypes
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/qmldir
        DESTINATION "tests/${PROJECT_NAME}")
endif()
