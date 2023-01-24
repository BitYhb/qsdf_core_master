#pragma once

#include "luawrapper.h"

#include <extensionsystem/iplugin.h>

#include <QSharedPointer>

namespace LuaSupport::Internal {

class LuaSupportPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qsdf-core.plugin" FILE "LuaSupport.json")

public:
    bool initialize(const QStringList &arguments, QString *errorString) override;

private:
    QSharedPointer<LuaWrapper> wrapper;
};

} // namespace LuaSupport::Internal
