#include "luawrapper.h"

namespace LuaSupport {

namespace Internal {
class LuaWrapperPrivate
{
public:
    void registerLuaClass();
};
} // namespace Internal

LuaWrapper::LuaWrapper(QObject *parent)
    : QObject(parent)
    , d(new Internal::LuaWrapperPrivate())
{}

LuaWrapper::~LuaWrapper() noexcept {}

} // namespace LuaSupport