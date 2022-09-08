#ifndef CORE_MAIN_WINDOW_H
#define CORE_MAIN_WINDOW_H

#include <QMainWindow>

namespace Core {

class ICore;

namespace Internal {

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    ICore *m_coreImpl = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // CORE_MAIN_WINDOW_H
