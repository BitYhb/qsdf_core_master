#ifndef CORE_SINGLESUBJECT_H
#define CORE_SINGLESUBJECT_H

#include "core_global.h"

#include <extensionsystem/isubject.h>

#include <QJsonObject>
#include <QMutex>

using namespace ExtensionSystem;

namespace Core {

class MIPS_COREPLUGIN_EXPORT SingleSubject : public ISubject
{
    SingleSubject() = default;

public:
    enum class MsgType {
        MSG_NONE,
    };

    struct Msg
    {
        MsgType msg_type = MsgType::MSG_NONE;
        QJsonObject msg;

        QJsonObject obj();
    };

    ~SingleSubject() override = default;

    static SingleSubject *instance();

    void attach(IObserver *observer) override;
    void detach(IObserver *observer) override;
    void notify() override;

    void createMessage(const Msg &message);
    void createMessage(MsgType type, const QJsonObject &msg);

private:
    QList<IObserver *> m_observers;
    Msg m_message; // current msg
    QMutex m_mutex;
};

} // namespace Core

#endif // CORE_SINGLESUBJECT_H
