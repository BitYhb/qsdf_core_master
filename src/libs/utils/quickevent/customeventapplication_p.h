#pragma once

#include <QMap>
#include <QReadWriteLock>

/// \cond INTERNAL
namespace Utils::Internal {

class CustomEventApplicationPrivate
{
    friend class Utils::CustomEventApplication;
public:
    explicit CustomEventApplicationPrivate(CustomEventApplication &object)
        : q(&object)
    {}

protected:
    CustomEventApplication *q;

private:
    static QMap<QByteArray, QMap<qint32, QObject *>> defaultEventQueue; // default level
    static QReadWriteLock eventQueueLock; // For event queues
};

} // namespace Utils::Internal
/// \endcond
