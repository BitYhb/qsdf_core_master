#ifndef CORE_MAIN_WINDOW_H
#define CORE_MAIN_WINDOW_H

#include <QQmlApplicationEngine>

namespace Core {

class ICore;

namespace Internal {

class MainWindow final : public QQmlApplicationEngine
{
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow() override;

private:
    ICore *m_coreImpl = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // CORE_MAIN_WINDOW_H
