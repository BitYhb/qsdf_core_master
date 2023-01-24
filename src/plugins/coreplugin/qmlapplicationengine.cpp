#include "qmlapplicationengine.h"

#include <app/app_version.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(mainWindow, "mips.coreplugin", QtWarningMsg)

namespace Core::Internal {

class QmlApplicationEnginePrivate
{};

QmlApplicationEngine::QmlApplicationEngine(QObject *parent)
    : QQmlApplicationEngine(parent)
{}

QmlApplicationEngine::~QmlApplicationEngine()
{

}

} // namespace Core::Internal
