#include "customeventapplication.h"
#include "customeventapplication_p.h"

namespace Utils {

namespace Internal {
QMap<QByteArray, QMap<qint32, QObject *>> CustomEventApplicationPrivate::defaultEventQueue; // default level
QReadWriteLock CustomEventApplicationPrivate::eventQueueLock;                               // For event queues
} // namespace Internal

Internal::CustomEventApplicationPrivate *d = nullptr;

CustomEventApplication::CustomEventApplication(QObject *parent)
    : QObject(parent)
{
    d = new Internal::CustomEventApplicationPrivate(*this);
}

QMap<QByteArray, QMap<qint32, QObject*>> CustomEventApplication::defaultEventQueue()
{
    return Internal::CustomEventApplicationPrivate::defaultEventQueue;
}

QReadWriteLock* CustomEventApplication::listLock()
{
    return &Internal::CustomEventApplicationPrivate::eventQueueLock;
}

int CustomEventApplication::methodIndex(QObject *obj, const QByteArray &methodName)
{
    return -1;
}

/*!
 * \brief This function is used to subscribe to the specified event.
 */
bool CustomEventApplication::subscribeEvent(QObject *listener, const QByteArray &eventName, qint32 level)
{
    using namespace Internal;
    QMap<QByteArray, QMap<qint32, QObject *>> &defaultEventQueue = CustomEventApplicationPrivate::defaultEventQueue;
    if (level < 0) { // default level
        QWriteLocker locker(&CustomEventApplicationPrivate::eventQueueLock);
        if (defaultEventQueue.contains(eventName)) {
            auto list = defaultEventQueue.value(eventName);
            list.insert((qint32) list.size(), listener);
            defaultEventQueue.insert(eventName, list);
            return true;
        }
        QMap<qint32, QObject *> list;
        list.insert(0, listener);
        defaultEventQueue.insert(eventName, list);
    }
    return true;
}

} // namespace Utils
