add_library(shared_singleapplication STATIC
    singleapplication.h singleapplication.cpp
    localpeer.h localpeer.cpp
    lockedfile.h lockedfile.cpp
)
target_link_libraries(shared_singleapplication
    Qt6::Core Qt6::Network Qt6::Widgets)
target_include_directories(shared_singleapplication
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(shared_singleapplication
    PRIVATE "SINGLEAPPLICATION_EXPORT=__declspec(dllexport)")
