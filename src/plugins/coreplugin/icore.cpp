#include "icore.h"
#include "mainqmlapplicationengine.h"

#include <app/app_version.h>
#include <extensionsystem/pluginmanager.h>

#include <QCoreApplication>

using namespace Utils;
using namespace ExtensionSystem;
using namespace Core::Internal;

namespace Core {

class ICorePrivate
{
    Q_DECLARE_PUBLIC(ICore)
public:
    ICorePrivate(ICore &object)
        : q_ptr(&object)
    {}

    MainQmlApplicationEngine *m_mainWindow = nullptr;

protected:
    ICore *q_ptr;
};

ICore::ICore(QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new ICorePrivate(*this))
{}

ICore::~ICore() {}

void ICore::setMainWindow(Internal::MainQmlApplicationEngine *mainWindow)
{
    Q_D(ICore);
    d->m_mainWindow = mainWindow;
}

ICore *ICore::instance()
{
    static ICore iCore;
    return &iCore;
}

QFileInfo ICore::resourcePath(const QString &relativePath)
{
    return QFileInfo(QCoreApplication::applicationDirPath() + '/' + RELATIVE_DATA_PATH + '/' + relativePath);
}

QFileInfo ICore::userResourcePath(const QString &relativePath)
{
    const QString configDir = QFileInfo(settings(QSettings::UserScope)->fileName()).path();
    const QString urp = configDir + '/' + QLatin1String(Constants::MIPS_ID);

    if (!QFileInfo::exists(urp + QLatin1Char('/'))) {
        QDir dir;
        if (!dir.mkpath(urp))
            qWarning() << "could not create" << urp;
    }

    return QFileInfo(urp + relativePath);
}

MipsSettings *ICore::settings(QSettings::Scope scope)
{
    if (scope == QSettings::UserScope)
        return PluginManager::settings();
    else
        return PluginManager::globalSettings();
}

void ICore::addContextObject(IContext *context)
{
    Q_D(ICore);
}

} // namespace Core
