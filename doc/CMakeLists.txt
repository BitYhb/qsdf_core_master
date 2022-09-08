## 下载Doxygen用于生成doc
set(DOXYGEN_DL_VERSION 1.9.5)
set(DOXYGEN_DL_FILENAME doxygen-${DOXYGEN_DL_VERSION}.windows.x64.bin.zip)
set(DOXYGEN_EXECUTABLE_PATH ${CMAKE_CURRENT_BINARY_DIR}/doxygen-${DOXYGEN_DL_VERSION})

set(DOXYGEN_COMMAND "${DOXYGEN_EXECUTABLE_PATH}/doxygen.exe" CACHE STRING "")

set(DL_DOXYGEN_URL https://www.doxygen.nl/files/${DOXYGEN_DL_FILENAME})

file(DOWNLOAD ${DL_DOXYGEN_URL} ${DOXYGEN_EXECUTABLE_PATH}/${DOXYGEN_DL_FILENAME}
    EXPECTED_MD5 8c3cf926178c2e4fc6a389e2cbb02762
    STATUS ERR)

if(ERR EQUAL 0)
    message(STATUS "Doxygen download succeeded, version is ${DOXYGEN_DL_VERSION}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${DOXYGEN_EXECUTABLE_PATH}/${DOXYGEN_DL_FILENAME}
        WORKING_DIRECTORY ${DOXYGEN_EXECUTABLE_PATH})
else()
    message(STATUS "Doxygen download failed")
    message(FATAL_ERROR "You can try downloading ${DL_DOXYGEN_URL} manually\n\t"
        "Or set DOXYGEN_COMMAND specifies the path of an existing Doxygen executable")
endif()

# 自定义 Doxygen 配置文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmakein")
    set(EXTENSION_TYPE_NO "NO")
    set(EXTENSION_TYPE_YES "YES")
    set(EXTENSION_TYPE_LIGHT "LIGHT")
    
    file(READ "Doxyfile.cmakein" doxyfile_in)
    string(REPLACE "$$DOXYGEN_PROJECT_NAME" "\${PROJECT_NAME}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_PROJECT_BRIEF" "Medical Image Processing Software" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_PROJECT_NUMBER" "\${MIPS_VERSION}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_INPUT" "\${CMAKE_SOURCE_DIR}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_RECURSIVE" "\${EXTENSION_TYPE_YES}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_AUTO_LIGHT" "\${EXTENSION_TYPE_LIGHT}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_GENERATE_LATEX" "\${EXTENSION_TYPE_NO}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_QUIET" "\${EXTENSION_TYPE_YES}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_INTERNAL_DOCS" "\${EXTENSION_TYPE_YES}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_CREATE_SUBDIRS" "\${EXTENSION_TYPE_YES}" doxyfile_in ${doxyfile_in})
    string(REPLACE "$$DOXYGEN_ENABLED_SECTIONS" "" doxyfile_in ${doxyfile_in})
    string(CONFIGURE "${doxyfile_in}" doxyfile)
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" CONTENT ${doxyfile})

    add_custom_command(OUTPUT doc_build
        COMMAND ${DOXYGEN_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${DOXYGEN_EXECUTABLE_PATH}
        VERBATIM)
    add_custom_target(doc ALL DEPENDS doc_build)
else()
    message(STATUS "The Doxygen configuration file is missing, and the doc generation step will be ignored")
endif()
