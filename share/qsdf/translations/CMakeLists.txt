set(languages zh_CN)

add_translation_targets(launch
    LANGUAGES ${languages}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
