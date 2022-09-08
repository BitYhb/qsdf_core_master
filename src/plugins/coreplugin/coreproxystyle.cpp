#include "coreproxystyle.h"

#include <QStyleFactory>

namespace Core {

CoreProxyStyle::CoreProxyStyle(const QString &key)
    : QProxyStyle(QStyleFactory::create(key))
{}

CoreProxyStyle::~CoreProxyStyle() {}

} // namespace Core
