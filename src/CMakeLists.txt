add_library(app_version INTERFACE)
target_include_directories(app_version
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${QSDF_HEADER_INSTALL_PATH}>
)
install(TARGETS app_version EXPORT QSDF)

add_subdirectory(app)
add_subdirectory(share)
add_subdirectory(shared)
add_subdirectory(libs)
add_subdirectory(plugins)
add_subdirectory(domains)

option(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE "Compilation tools" OFF)
mark_as_advanced(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)

if (${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)
    add_subdirectory(utility_software)
endif()

install(EXPORT QSDF
    DESTINATION ${QSDF_CMAKE_INSTALL_PATH}/QSDF
    COMPONENT Devel EXCLUDE_FROM_ALL
    NAMESPACE QSDF::
    FILE QSDFTargets.cmake
)

file(WRITE ${CMAKE_BINARY_DIR}/cmake/QSDFConfig.cmake "
include(CMakeFindDependencyMacro)
find_dependency(Qt6 ${QSDF_QT_VERSION_MIN} COMPONENTS Core Gui Widgets REQUIRED)

if (NOT DEFINED add_qsdf_plugin)
    include(\${CMAKE_CURRENT_LIST_DIR}/FrameworkAPI.cmake)
endif()

if (NOT TARGET MIPSortware::Core)
    include(\${CMAKE_CURRENT_LIST_DIR}/QSDFTargets.cmake)
endif()
")

# 导出本地生成的 QSDFTargets.cmake
export(EXPORT QSDF
    NAMESPACE QSDF::
    FILE ${CMAKE_BINARY_DIR}/cmake/QSDFTargets.cmake
)

file(COPY
    ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPI.cmake
    ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPIInternal.cmake
    DESTINATION ${CMAKE_BINARY_DIR}/cmake
)

# 开发包
install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPI.cmake
    ${PROJECT_SOURCE_DIR}/cmake/FrameworkAPIInternal.cmake
    DESTINATION ${QSDF_CMAKE_INSTALL_PATH}/QSDF
    COMPONENT Devel EXCLUDE_FROM_ALL
)
