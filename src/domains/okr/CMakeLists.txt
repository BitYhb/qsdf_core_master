cmake_minimum_required(VERSION 3.21)

project(DPM LANGUAGES CXX)

string(TOLOWER "${CMAKE_PROJECT_NAME}" _proj_binary_file_name)

qt_add_qml_module(${_proj_binary_file_name}
    URL ${_proj_binary_file_name}
    VERSION 1.0
    QML_FILES src/qml/main.qml)
