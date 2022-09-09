#include "mainwindow.h"

#include <app/app_version.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(mainWindow, "mips.coreplugin", QtWarningMsg)

using namespace Core::Internal;

MainWindow::MainWindow(QObject *parent)
    : QQmlApplicationEngine(parent)
{
    // setWindowTitle(QLatin1String(Constants::MIPS_DISPLAY_NAME));
}

MainWindow::~MainWindow() = default;
