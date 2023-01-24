#include "luasupportplugin.h"

#include <QDebug>

namespace LuaSupport::Internal {

bool LuaSupportPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    qDebug() << Q_FUNC_INFO;
    return false;
}

} // namespace LuaSupport::Internal
