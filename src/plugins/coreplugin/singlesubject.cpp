#include "singlesubject.h"

#include <extensionsystem/iobserver.h>
#include <utils/algorithm.h>

namespace Core {

QJsonObject SingleSubject::Msg::obj()
{
    msg.insert(QLatin1String("msg_type"), (int) msg_type);
    return msg;
}

Core::SingleSubject *SingleSubject::instance()
{
    static SingleSubject singleSubject;
    return &singleSubject;
}

void SingleSubject::attach(IObserver *observer)
{
    // What about repeated observers?

    if (observer == nullptr) {
        qWarning("Attempted to register an invalid observer");
        return;
    }
    m_observers.push_back(observer);
}

void SingleSubject::detach(IObserver *observer)
{
    m_observers.removeAll(observer);
}

void SingleSubject::notify()
{
    foreach (IObserver *observer, m_observers) {
        if (observer != nullptr) {
            observer->update(m_message.obj());
        }
    }
}

void SingleSubject::createMessage(const Msg &message)
{
    QMutexLocker locker(&m_mutex);
    m_message = message;
    notify();
}

void SingleSubject::createMessage(MsgType type, const QJsonObject &msg)
{
    QMutexLocker locker(&m_mutex);
    m_message.msg_type = type;
    m_message.msg = msg;
    notify();
}

} // namespace Core
