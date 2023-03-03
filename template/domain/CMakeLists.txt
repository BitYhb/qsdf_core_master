cmake_minimum_required(VERSION 3.16)

project(${TM_TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

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

file(GLOB_RECURSE abs_qml_sources CONFIGURE_DEPENDS "src/qml/*.qml")
set(rel_qml_sources "")
foreach(f_abs IN LISTS abs_qml_sources)
    file(RELATIVE_PATH f_rel "${CMAKE_CURRENT_LIST_DIR}" "${f_abs}")
    list(APPEND rel_qml_sources "${f_rel}")
endforeach()

file(GLOB_RECURSE abs_png_resources CONFIGURE_DEPENDS "src/image/*.png")
set(rel_png_resources "")
foreach(f_abs IN LISTS abs_png_resources)
    file(RELATIVE_PATH f_rel "${CMAKE_CURRENT_LIST_DIR}" "${f_abs}")
    list(APPEND rel_png_resources "${f_rel}")
endforeach()

qt_add_qml_module(${PROJECT_NAME}
    URI ${TM_TARGET_NAME}
    VERSION 1.0
    RESOURCES ${rel_png_resources}
    QML_FILES ${rel_qml_sources})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Quick QSDF::Utils)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(${PROJECT_NAME}_ENABLE_TESTS "Enabled for testing." OFF)
if(${${PROJECT_NAME}_ENABLE_TESTS})
    add_subdirectory(tests)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS ${PROJECT_NAME} DESTINATION "tests")
    install(TARGETS ${PROJECT_NAME}plugin DESTINATION "tests/${PROJECT_NAME}")
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.qmltypes
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/qmldir
        DESTINATION "tests/${PROJECT_NAME}")
else()
    if(NOT ANDROID)
        install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}")
        install(TARGETS ${PROJECT_NAME}plugin DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}")
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.qmltypes
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/qmldir
            DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}")
    else()
        install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}plugin DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif()
endif()
