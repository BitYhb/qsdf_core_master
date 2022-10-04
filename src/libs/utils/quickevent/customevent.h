#pragma once
#include "utils_export.h"

#include <QEvent>
#include <QList>
#include <QSharedPointer>

#define EVENT_METHOD_PREFIX "event"

namespace Utils {

namespace Internal {
class CustomEventPrivate;
}

class QUICK_UTILS_EXPORT CustomEvent final : public QEvent
{
public:
    explicit CustomEvent(Type type = GenericEvent);
    CustomEvent(const QByteArray &eventName, const QList<QSharedPointer<QVariant>> &info, Type type = GenericEvent);
    ~CustomEvent() override;

    [[nodiscard]] QByteArray eventName() const;
    void setEventName(const QByteArray &eventName);

    [[nodiscard]] QList<QSharedPointer<QVariant>> info() const;
    void setInfo(const QList<QSharedPointer<QVariant>> &info);

    static QEvent::Type GenericEvent;

protected:
    QSharedPointer<Internal::CustomEventPrivate> d_ptr;

private:
    Q_EVENT_DISABLE_COPY(CustomEvent);
};

} // namespace Utils
