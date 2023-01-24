add_qsdf_plugin(LuaSupport
    DEPENDS Qt::Core
    PUBLIC_DEPENDS Core
    MSVC_SOLUTION_FOLDER "${PLUGINS_SOLUTION_FOLDER}"
    SOURCES
        luasupportplugin.h luasupportplugin.cpp
        luawrapper.h luawrapper.cpp)
