#ifndef QSDF_CORE_PLUGIN_CORE_FRAMELESSQUICKWINDOW_H
#define QSDF_CORE_PLUGIN_CORE_FRAMELESSQUICKWINDOW_H

#include <QQuickWindow>

namespace Core {
namespace Internal {

class FramelessQuickWindow : public QQuickWindow
{
    Q_OBJECT
public:
    explicit FramelessQuickWindow(QWindow *parent = nullptr);
    ~FramelessQuickWindow() override;

private slots:
    void eventUpdateFullScreenMode();
};

} // namespace Internal
} // namespace Core

#endif // QSDF_CORE_PLUGIN_CORE_FRAMELESSQUICKWINDOW_H
