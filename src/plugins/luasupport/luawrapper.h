#pragma once

#include <QObject>

namespace LuaSupport {

namespace Internal {
class LuaWrapperPrivate;
}

class LuaWrapper : public QObject
{
    Q_OBJECT
public:
    explicit LuaWrapper(QObject *parent = nullptr);
    ~LuaWrapper() override;

    void registerToLua(QObject* object);

private:
    std::unique_ptr<Internal::LuaWrapperPrivate> d;
};

} // namespace LuaSupport
