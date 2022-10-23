#ifndef QUICK_UTILS_CUSTOM_EVENT_APPLICATION_H
#define QUICK_UTILS_CUSTOM_EVENT_APPLICATION_H

#include "utils_export.h"

#include <QObject>
#include <QSharedPointer>

class QReadWriteLock;

namespace Utils {

namespace Internal {
class CustomEventApplicationPrivate;
}

class QUICK_UTILS_EXPORT CustomEventApplication : public QObject
{
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
    Q_DISABLE_COPY(CustomEventApplication)
};

} // namespace Utils

#include "customeventapplication.tpp"

#endif // QUICK_UTILS_CUSTOM_EVENT_APPLICATION_H
