#include "customquickevent.h"

namespace Utils {

/// \cond INTERNAL
namespace Internal {
class CustomEventPrivate
{
public:
    explicit CustomEventPrivate(CustomQuickEvent &object)
        : q_ptr(&object)
    {}

    QByteArray eventName;
    QList<QSharedPointer<QVariant>> info;

protected:
    CustomQuickEvent *q_ptr = nullptr;
};
} // namespace Internal
/// \endcond

/*!
 * \class CustomQuickEvent
 * \brief The CustomQuickEvent class contains parameters that can be used to describe all events.
 *
 * Quick events occurs when MIPSoftware::publishEvent() function is called.
 */

QEvent::Type CustomQuickEvent::GenericEvent = static_cast<QEvent::Type>(QEvent::registerEventType());

CustomQuickEvent::CustomQuickEvent(QEvent::Type type)
    : QEvent(type)
    , d_ptr(new Internal::CustomEventPrivate(*this))
{}

CustomQuickEvent::~CustomQuickEvent() = default;

CustomQuickEvent::CustomQuickEvent(const QByteArray &eventName, const QList<QSharedPointer<QVariant>> &info, QEvent::Type type)
    : QEvent(type)
{
    d_ptr->eventName = eventName;
    d_ptr->info = info;
}

QByteArray CustomQuickEvent::eventName() const
{
    return d_ptr->eventName;
}

void CustomQuickEvent::setEventName(const QByteArray &eventName)
{
    d_ptr->eventName = eventName;
}

QList<QSharedPointer<QVariant>> CustomQuickEvent::info() const
{
    return d_ptr->info;
}

void CustomQuickEvent::setInfo(const QList<QSharedPointer<QVariant>> &info)
{
    d_ptr->info = info;
}

} // namespace Utils
