set(proj ${TM_TARGET_NAME}_ui_test)

qt_add_executable(${proj}
    main.cpp)

qt_add_qml_module(${proj}
    URI ${TM_TARGET_NAME}_ui_test
    VERSION 1.0
    QML_FILES main.qml)

target_link_libraries(${proj}
    PRIVATE Qt6::Quick ${TM_TARGET_NAME})
