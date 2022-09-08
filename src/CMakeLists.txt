add_library(app_version INTERFACE)
target_include_directories(app_version
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${MIPS_HEADER_INSTALL_PATH}>
)
install(TARGETS app_version EXPORT MIPSoftware)

add_subdirectory(app)
add_subdirectory(share)
add_subdirectory(shared)
add_subdirectory(libs)
add_subdirectory(plugins)

option(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE "Compilation tools" OFF)
mark_as_advanced(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)

if (${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)
    add_subdirectory(utility_software)
endif()

install(EXPORT MIPSoftware
    DESTINATION ${MIPS_CMAKE_INSTALL_PATH}/MIPSoftware
    COMPONENT Devel EXCLUDE_FROM_ALL
    NAMESPACE MIPSoftware::
    FILE MIPSoftwareTargets.cmake
)

file(WRITE ${CMAKE_BINARY_DIR}/cmake/MIPSoftwareConfig.cmake "
include(CMakeFindDependencyMacro)
find_dependency(Qt6 ${MIPS_QT_VERSION_MIN} COMPONENTS Core Gui Widgets REQUIRED)

if (NOT DEFINED add_mips_plugin)
    include(\${CMAKE_CURRENT_LIST_DIR}/MIPSoftwareAPI.cmake)
endif()

if (NOT TARGET MIPSortware::Core)
    include(\${CMAKE_CURRENT_LIST_DIR}/MIPSoftwareTargets.cmake)
endif()
")

# 导出本地生成的 MIPSoftwareTargets.cmake
export(EXPORT MIPSoftware
    NAMESPACE MIPSoftware::
    FILE ${CMAKE_BINARY_DIR}/cmake/MIPSoftwareTargets.cmake
)

file(COPY
    ${PROJECT_SOURCE_DIR}/cmake/MIPSoftwareAPI.cmake
    ${PROJECT_SOURCE_DIR}/cmake/MIPSoftwareAPIInternal.cmake
    DESTINATION ${CMAKE_BINARY_DIR}/cmake
)

# 开发包
install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/MIPSoftwareAPI.cmake
    ${PROJECT_SOURCE_DIR}/cmake/MIPSoftwareAPIInternal.cmake
    DESTINATION ${MIPS_CMAKE_INSTALL_PATH}/MIPSoftware
    COMPONENT Devel EXCLUDE_FROM_ALL
)
