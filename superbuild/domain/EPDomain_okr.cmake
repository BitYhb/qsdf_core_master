set(domain_proj EPDomain_okr)

set(${domain_proj}_DEPENDENCIES)

ExternalProject_Include_Dependencies(${domain_proj} PROJECT_VAR domain_proj DEPENDS_VAR ${domain_proj}_DEPENDENCIES)

_sb_get_domain_source_path(domain_proj _proj_source_path)

ExternalProject_Add(${domain_proj}
    ${${domain_proj}_EP_ARGS}
    SOURCE_DIR ${_proj_source_path}
    BINARY_DIR ${CMAKE_BINARY_DIR}/${QSDF_BINARY_INNER_SUBDIR}
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_DIR "")
