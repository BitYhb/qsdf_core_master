#ifndef EXTENSIONSYSTEM_ISUBJECT_H
#define EXTENSIONSYSTEM_ISUBJECT_H

#include "extensionsystem_global.h"

namespace ExtensionSystem {

class IObserver;
class MIPS_EXTENSIONSYSTEM_EXPORT ISubject
{
public:
    virtual ~ISubject() = default;

    virtual void attach(IObserver *observer) = 0;
    virtual void detach(IObserver *observer) = 0;
    virtual void notify() = 0;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_ISUBJECT_H
