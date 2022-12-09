#include "framelessquickwindow.h"

#include <utils/mipssettings.h>

namespace Core::Internal {

FramelessQuickWindow::FramelessQuickWindow(QWindow *parent)
    : QQuickWindow(parent)
{}

FramelessQuickWindow::~FramelessQuickWindow() {}

void FramelessQuickWindow::eventUpdateFullScreenMode()
{
    if (Plugin)
}

} // namespace Core::Internal
