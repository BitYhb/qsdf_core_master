#include "qmlapplicationengine.h"

#include <app/app_version.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(mainWindow, "mips.coreplugin", QtWarningMsg)

using namespace Core::Internal;

QmlApplicationEngine::QmlApplicationEngine(QObject *parent)
    : QQmlApplicationEngine(parent)
{
    // setWindowTitle(QLatin1String(Constants::MIPS_DISPLAY_NAME));
}

QmlApplicationEngine::~QmlApplicationEngine() = default;
