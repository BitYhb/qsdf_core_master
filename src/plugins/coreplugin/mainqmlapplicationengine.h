#ifndef CORE_MAIN_WINDOW_H
#define CORE_MAIN_WINDOW_H

#include <QQmlApplicationEngine>

namespace Core {

class ICore;

namespace Internal {

class MainQmlApplicationEngine final : public QQmlApplicationEngine
{
    Q_OBJECT
public:
    explicit MainQmlApplicationEngine(QObject *parent = nullptr);
    ~MainQmlApplicationEngine() override;

private:
    ICore *m_coreImpl = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // CORE_MAIN_WINDOW_H
