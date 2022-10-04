#include "customevent.h"

namespace Utils {

/// \cond INTERNAL
namespace Internal {
class CustomEventPrivate
{
public:
    explicit CustomEventPrivate(CustomEvent &object)
        : q_ptr(&object)
    {}

    QByteArray eventName;
    QList<QSharedPointer<QVariant>> info;

protected:
    CustomEvent *q_ptr = nullptr;
};
} // namespace Internal
/// \endcond

/*!
 * \class CustomEvent
 * \brief The CustomEvent class contains parameters that can be used to describe all events.
 *
 * Quick events occurs when MIPSoftware::publishEvent() function is called.
 */

QEvent::Type CustomEvent::GenericEvent = static_cast<QEvent::Type>(QEvent::registerEventType());

CustomEvent::CustomEvent(QEvent::Type type)
    : QEvent(type)
    , d_ptr(new Internal::CustomEventPrivate(*this))
{}

CustomEvent::~CustomEvent() = default;

CustomEvent::CustomEvent(const QByteArray &eventName, const QList<QSharedPointer<QVariant>> &info, QEvent::Type type)
    : QEvent(type)
{
    d_ptr->eventName = eventName;
    d_ptr->info = info;
}

QByteArray CustomEvent::eventName() const
{
    return d_ptr->eventName;
}

void CustomEvent::setEventName(const QByteArray &eventName)
{
    d_ptr->eventName = eventName;
}

QList<QSharedPointer<QVariant>> CustomEvent::info() const
{
    return d_ptr->info;
}

void CustomEvent::setInfo(const QList<QSharedPointer<QVariant>> &info)
{
    d_ptr->info = info;
}

} // namespace Utils
