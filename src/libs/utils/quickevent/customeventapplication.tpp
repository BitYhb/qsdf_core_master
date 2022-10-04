#pragma once
#include "customeventapplication_p.h"

#include <utils/macrodefinition.h>
#include <utils/mipsassert.h>
#include <utils/quickevent/quickevent.h>
#include <utils/quickapplication.h>

#include <QMetaMethod>
#include <QThread>

namespace Utils {

template<typename T>
static void getList(QList<QByteArray> &typeNames, QList<QGenericArgument> &list, T &&t)
{
    typeNames << QByteArray(typeid(t).name());
    list << Q_ARG(T, t);
}

template<typename T>
static void getList(QList<QByteArray> &typeNames, QList<QSharedPointer<QVariant>> &list, T &&t)
{
    typeNames << QByteArray(typeid(t).name());
    QSharedPointer<QVariant> ptr(new QVariant);
    ptr->setValue(t);
    list << ptr;
}

/*!
 * \brief Publish an event that notifies all objects subscribed to the \a eventName event.
 *
 * If the event \a eventName sender and listener in the same thread or connection type \a type for the
 * Qt::BlockingQueuedConnection, the event handler will block the call. If the event handler needs to use
 * non-blocking way call, you need to make the \a eventName event sender and listener in different threads
 * and connections can't be a Qt::BlockingQueuedConnection.
 */
template<class... Args>
void CustomEventApplication::publishEvent(const QByteArray &eventName, Qt::ConnectionType type, Args &&...args)
{
    if (eventName.isEmpty()) {
        return;
    }

    auto argsCount = sizeof...(args);
    QUICK_ASSERT(argsCount <= 10, qCWarning(lcUtils, "publishEvent() parameter-pack args cannot exceed 10"); return);

    QReadLocker locker(&Internal::CustomEventApplicationPrivate::eventQueueLock);
    const auto &defaultEventQueue = Internal::CustomEventApplicationPrivate::defaultEventQueue;
    if (!defaultEventQueue.contains(eventName)) {
        return;
    }

    auto objs = defaultEventQueue[eventName].values();
    const auto methodName = QByteArray(EVENT_METHOD_PREFIX) + eventName;
    foreach (auto item, objs) {
        if (item->thread() == QThread::currentThread() || type == Qt::BlockingQueuedConnection) {
            QList<QByteArray> typeNames;
            QList<QGenericArgument> g;
            std::initializer_list<int32_t>{(getList(typeNames, g, args), 0)...};
            while (g.size() < 10) {
                g << QGenericArgument();
            }

            int index = methodIndex(item, methodName);
            auto method = item->metaObject()->method(index);
            if (item->thread() == QThread::currentThread()) {
                method.invoke(item, Qt::DirectConnection, g[0], g[1], g[2], g[3], g[4], g[5], g[6], g[7], g[8], g[9]);
            } else {
                method.invoke(item, type, g[0], g[1], g[2], g[3], g[4], g[5], g[6], g[7], g[8], g[9]);
            }
        } else {
            QList<QByteArray> typeNames;
            QList<QSharedPointer<QVariant>> list;
            std::initializer_list<int32_t>{(getList(typeNames, list, args), 0)...};

            auto quickEvent = new CustomEvent;
            quickEvent->setInfo(list);
            quickEvent->setEventName(eventName);
            QCoreApplication::postEvent(item, quickEvent);
        }
    }
}


} // namespace Utils
