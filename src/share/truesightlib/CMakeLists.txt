set(EXT_TOOLS_DIR "${MIPS_DATA_PATH}/externaltools")

set(tools truesightlib.xml)

mips_copy_to_builddir(copy_src_share_to_builddir
    FILES ${tools}
    DESTINATION "${EXT_TOOLS_DIR}"
)

install(FILES ${tools}
    DESTINATION "${EXT_TOOLS_DIR}"
)
