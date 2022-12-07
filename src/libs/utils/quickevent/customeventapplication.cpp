#include "customeventapplication.h"

namespace Utils {

QMap<QByteArray, QMap<qint32, QObject *>> CustomEventApplication::m_defaultEventQueue; // default level
QReadWriteLock CustomEventApplication::m_eventQueueLock;                               // For event queues

CustomEventApplication::CustomEventApplication(QObject *parent)
    : QObject(parent)
{
}

QMap<QByteArray, QMap<qint32, QObject*>> CustomEventApplication::defaultEventQueue()
{
    return m_defaultEventQueue;
}

QReadWriteLock* CustomEventApplication::listLock()
{
    return &m_eventQueueLock;
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
    if (level < 0) { // default level
        QWriteLocker locker(&m_eventQueueLock);
        if (m_defaultEventQueue.contains(eventName)) {
            auto list = m_defaultEventQueue.value(eventName);
            list.insert((qint32) list.size(), listener);
            m_defaultEventQueue.insert(eventName, list);
            return true;
        }
        QMap<qint32, QObject *> list;
        list.insert(0, listener);
        m_defaultEventQueue.insert(eventName, list);
    }
    return true;
}

} // namespace Utils
