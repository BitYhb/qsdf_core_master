#include "framelessquickwindow.h"

#include <coreplugin/corepluginmacro.h>
#include <coreplugin/qmlapplicationengine.h>
#include <coreplugin/coreplugin.h>

#include <QQmlEngineExtensionPlugin>

namespace Core::Internal {

class FramelessQuickWindowPrivate
{
public:
    QString domainViewURI;
};

FramelessQuickWindow::FramelessQuickWindow(QWindow *parent)
    : QQuickWindow(parent)
    , d(std::make_unique<FramelessQuickWindowPrivate>())
{}

FramelessQuickWindow::~FramelessQuickWindow() = default;

void FramelessQuickWindow::classBegin() {}

void FramelessQuickWindow::componentComplete()
{
    d->domainViewURI = CorePlugin::instance()->domainViewerURI();
    emit domainViewURIChanged();
}

void FramelessQuickWindow::eventUpdateFullScreenMode() {}

QString FramelessQuickWindow::domainViewURI() const
{
    return d->domainViewURI;
}

} // namespace Core::Internal
