add_library(app_version INTERFACE)
target_include_directories(app_version
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${QSDF_HEADER_INSTALL_PATH}>)
install(TARGETS app_version EXPORT QSDFTargets)

add_subdirectory(app)
add_subdirectory(share)
add_subdirectory(shared)
add_subdirectory(libs)
add_subdirectory(plugins)

option(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE "Compilation tools" OFF)
mark_as_advanced(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)

if(${PROJECT_NAME}_BUILD_UTILITY_SOFTWARE)
    add_subdirectory(utility_software)
endif()
