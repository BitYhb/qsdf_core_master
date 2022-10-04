#pragma once
#include "utils_export.h"

#include <QObject>
#include <QSharedPointer>

namespace Utils {

namespace Internal {
class CustomEventApplicationPrivate;
}

class QUICK_UTILS_EXPORT CustomEventApplication : public QObject
{
public:
    explicit CustomEventApplication(QObject* parent = nullptr);
    ~CustomEventApplication() override;

    static int methodIndex(QObject* obj, const QByteArray &methodName);
    static bool subscribeEvent(QObject *listener, const QByteArray &eventName, qint32 level = -1);

    template<class... Args>
    static void publishEvent(const QByteArray &eventName, Qt::ConnectionType type, Args &&...args);

protected:
    QSharedPointer<Internal::CustomEventApplicationPrivate> d;
};

} // namespace Utils

#include "customeventapplication.tpp"
