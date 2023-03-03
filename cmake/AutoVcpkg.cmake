set(AUTO_VCPKG_GIT_REPOSITORY git@github.com:microsoft/vcpkg.git)
set(AUTO_VCPKG_GIT_TAG 2023.01.09)

function(__AutoVcpkg_vcpkg_download)
    ExternalProject_Add(vcpkg
        GIT_REPOSITORY ${AUTO_VCPKG_GIT_REPOSITORY}
        GIT_TAG ${AUTO_VCPKG_GIT_TAG} # latest release version tag
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vcpkg
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vcpkg/src/vcpkg/bootstrap-vcpkg.bat # for Windows
        INSTALL_COMMAND "")

    # Get the path to the vcpkg binary
    ExternalProject_Get_Property(vcpkg SOURCE_DIR)
    set(VCPKG_EXECUTABLE ${SOURCE_DIR}/vcpkg)
endfunction()

__AutoVcpkg_vcpkg_download()