#ifndef QSDF_LIBS_UTILS_QUICK_EVENT_CUSTOM_EVENT_APPLICATION_H
#define QSDF_LIBS_UTILS_QUICK_EVENT_CUSTOM_EVENT_APPLICATION_H

#include "utils_export.h"

#include <QObject>
#include <QSharedPointer>

class QReadWriteLock;

namespace Utils {

class QUICK_UTILS_EXPORT CustomEventApplication : public QObject
{
    Q_DISABLE_COPY(CustomEventApplication)
public:
    explicit CustomEventApplication(QObject *parent = nullptr);
    ~CustomEventApplication() override = default;

    static QMap<QByteArray, QMap<qint32, QObject *>> defaultEventQueue();
    static QReadWriteLock *listLock();
    static int methodIndex(QObject *obj, const QByteArray &methodName);

    static bool subscribeEvent(QObject *listener, const QByteArray &eventName, qint32 level = -1);

    template<class... Args>
    static void publishEvent(const QByteArray &eventName, Qt::ConnectionType type, Args &&...args);

private:
    static QMap<QByteArray, QMap<qint32, QObject *>> m_defaultEventQueue; // default level
    static QReadWriteLock m_eventQueueLock;                               // For event queues
};

} // namespace Utils

#include "customeventapplication.tpp"

#endif
