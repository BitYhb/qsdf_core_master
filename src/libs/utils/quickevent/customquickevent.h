#ifndef QSDF_LIBS_UTILS_QUICK_EVENT_CUSTOM_QUICK_EVENT_H
#define QSDF_LIBS_UTILS_QUICK_EVENT_CUSTOM_QUICK_EVENT_H

#include "utils_export.h"

#include <QEvent>
#include <QList>
#include <QSharedPointer>

#define QUICK_EVENT_METHOD_PREFIX "event"

namespace Utils {

namespace Internal {
class CustomEventPrivate;
}

class QUICK_UTILS_EXPORT CustomQuickEvent final : public QEvent
{
public:
    explicit CustomQuickEvent(Type type = GenericEvent);
    CustomQuickEvent(const QByteArray &eventName, const QList<QSharedPointer<QVariant>> &info, Type type = GenericEvent);
    ~CustomQuickEvent() override;

    [[nodiscard]] QByteArray eventName() const;
    void setEventName(const QByteArray &eventName);

    [[nodiscard]] QList<QSharedPointer<QVariant>> info() const;
    void setInfo(const QList<QSharedPointer<QVariant>> &info);

    static QEvent::Type GenericEvent;

protected:
    QSharedPointer<Internal::CustomEventPrivate> d_ptr;

private:
    Q_EVENT_DISABLE_COPY(CustomQuickEvent);
};

} // namespace Utils

#endif
