#ifndef EXTENSIONSYSTEM_IOBSERVER_H
#define EXTENSIONSYSTEM_IOBSERVER_H

#include "extensionsystem_global.h"

namespace ExtensionSystem {

class MIPS_EXTENSIONSYSTEM_EXPORT IObserver
{
public:
    virtual ~IObserver() = default;

    virtual void update(const QJsonObject &message) = 0;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_IOBSERVER_H
