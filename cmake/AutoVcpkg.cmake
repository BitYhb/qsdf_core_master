set(AUTO_VCPKG_GIT_REPOSITORY git@github.com:microsoft/vcpkg.git)
set(AUTO_VCPKG_GIT_TAG 2023.01.09)

function(__AutoVcpkg_vcpkg_download)
    ExternalProject_Add(vcpkg
        GIT_REPOSITORY ${AUTO_VCPKG_GIT_REPOSITORY}
        GIT_TAG ${AUTO_VCPKG_GIT_TAG}
        GIT_SHALLOW true
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND "")
endfunction()

function(__AutoVcpkg_vcpkg_install)
endfunction()