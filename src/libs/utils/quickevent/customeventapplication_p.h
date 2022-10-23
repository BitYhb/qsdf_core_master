#pragma once

#include <QMap>
#include <QReadWriteLock>

class QObject;

/// \cond INTERNAL
namespace Utils {
class CustomEventApplication;
namespace Internal {

class CustomEventApplicationPrivate
{
    friend class Utils::CustomEventApplication;

public:
    explicit CustomEventApplicationPrivate(Utils::CustomEventApplication &object)
        : q(&object)
    {}

protected:
    Utils::CustomEventApplication *q;

private:
    static QMap<QByteArray, QMap<qint32, QObject *>> defaultEventQueue; // default level
    static QReadWriteLock eventQueueLock;                               // For event queues
};

} // namespace Utils::Internal
}
/// \endcond
