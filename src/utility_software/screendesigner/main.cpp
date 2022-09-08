#include "mainwindow.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/mipssettings.h>

#include <QApplication>
#include <QDir>
#include <QScreen>

using namespace ExtensionSystem;

const char SD_ORGANIZATION_NAME[] = "MIPSoftware";
const char SD_APPLICATION_NAME[] = "ScreenDesigner";

static inline Utils::MipsSettings *createUserSettings()
{
    return new Utils::MipsSettings(Utils::MipsSettings::IniFormat,
                                   Utils::MipsSettings::UserScope,
                                   QLatin1String(SD_ORGANIZATION_NAME),
                                   QLatin1String(SD_APPLICATION_NAME));
}

static inline QStringList getPluginPaths()
{
    qDebug() << QCoreApplication::applicationDirPath();
    QStringList pluginPaths{QDir::cleanPath(QCoreApplication::applicationDirPath() + '/' + RELATIVE_PLUGIN_PATH)};
    return pluginPaths;
}

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName(QLatin1String(SD_ORGANIZATION_NAME));
    QCoreApplication::setApplicationName(QLatin1String(SD_APPLICATION_NAME));

    QApplication app(argc, argv);

    Utils::MipsSettings *userSettings = createUserSettings();

    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("org.truesight.MIPS.MIPSoftwarePlugin"));
    PluginManager::setSettings(userSettings);

    // Must be set before setPluginPaths() call
    PluginManager::setAttribute(PluginManager::PluginManagerAttribute::LM_ABRIDGED);

    // Load Plugins
    const QStringList pluginPaths = getPluginPaths();
    PluginManager::setPluginPaths(pluginPaths);
    PluginManager::loadPlugins();

    ScreenDesigner::MainWindow mainWindow;
    mainWindow.loadTrueSightLibs();
    mainWindow.showMaximized();

    return app.exec();
}
