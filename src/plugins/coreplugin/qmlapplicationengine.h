#ifndef QUICK_QML_APPLICATION_ENGINE_H
#define QUICK_QML_APPLICATION_ENGINE_H

#include <utils/assert.h>
#include <utils/quickevent/quickevent.h>

#include <QQmlApplicationEngine>

namespace Core {

class ICore;

namespace Internal {

class QmlApplicationEngine final : public QQmlApplicationEngine
{
    Q_OBJECT
    QSDF_QUICK_EVENT_SUPPORT(QQmlApplicationEngine)

public:
    explicit QmlApplicationEngine(QObject *parent = nullptr);
    ~QmlApplicationEngine() override;

private:
    Core::ICore *m_coreImpl = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // QUICK_QML_APPLICATION_ENGINE_H
