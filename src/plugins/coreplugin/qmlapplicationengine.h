#ifndef QUICK_QML_APPLICATION_ENGINE_H
#define QUICK_QML_APPLICATION_ENGINE_H

#include <QQmlApplicationEngine>

#include <utils/quickevent/quickevent.h>

namespace Core {

class ICore;

namespace Internal {

class QmlApplicationEngine final : public QQmlApplicationEngine
{
    Q_OBJECT
    QUICK_EVENT(QQmlApplicationEngine)
public:
    explicit QmlApplicationEngine(QObject *parent = nullptr);
    ~QmlApplicationEngine() override;

private:
    Core::ICore *m_coreImpl = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // QUICK_QML_APPLICATION_ENGINE_H
