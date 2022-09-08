#include "mainwindow.h"

#include <app/app_version.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(mainWindow, "mips.coreplugin", QtWarningMsg)

namespace Core {
namespace Internal {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String(Constants::MIPS_DISPLAY_NAME));
}

MainWindow::~MainWindow() {}

} // namespace Internal
} // namespace Core
