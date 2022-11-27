set(resource_directories
    themes
    fonts
)

# copy resource directories during build
qsdf_copy_to_builddir(copy_share_to_builddir
    DIRECTORIES ${resource_directories}
    DESTINATION "${QSDF_DATA_PATH}"
    CREATE_SUBDIRS
)

add_subdirectory(translations)
