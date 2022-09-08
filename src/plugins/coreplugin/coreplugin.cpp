#include "coreplugin.h"
#include "mainwindow.h"
#include "themechooser.h"

#include <extensionsystem/pluginerroroverview.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <utils/mimetypes.h>
#include <utils/stringutils.h>
#include <utils/theme/theme.h>
#include <utils/theme/theme_p.h>

#include <QJsonObject>
#include <QMessageBox>
#include <QMetaEnum>

using namespace Utils;

namespace Core {
namespace Internal {

static CorePlugin *m_instance = nullptr;

CorePlugin::CorePlugin()
{
    m_instance = this;
}

CorePlugin::~CorePlugin()
{
    delete m_instance;
    setApplicationTheme(nullptr);
}

CorePlugin *CorePlugin::instance()
{
    return m_instance;
}

struct CoreArguments
{
    QColor overrideColor;
    Id themeId;
    bool presentationMode = false;
    bool designerMode = false; // 布局设计模式, 将禁用插件通信和交互
    QString patientId;
    QString caseId;
};

CoreArguments parseArguments(const QStringList &arguments)
{
    using namespace ExtensionSystem;
    using PMAttr = PluginManager::PluginManagerAttribute;

    CoreArguments args;
    const QMetaObject pmmo = PluginManager::staticMetaObject;
    const QMetaEnum pmme = pmmo.enumerator(pmmo.indexOfEnumerator("PluginManagerAttribute"));
    for (int i = 0; i < arguments.size(); ++i) {
        if (arguments.at(i) == QLatin1String("-plugin-manager-attribute")) {
            static const QString abridged = QLatin1String(pmme.valueToKey(static_cast<int>(PMAttr::LM_ABRIDGED)));
            args.designerMode = QString::compare(arguments.at(i + 1), abridged, Qt::CaseInsensitive) == 0;
            i++; // skip
        }
        if (arguments.at(i) == QLatin1String("-patient-id")) {
            args.patientId = arguments.at(i + 1);
            i++; // skip
        }
        if (arguments.at(i) == QLatin1String("-case-id")) {
            args.caseId = arguments.at(i + 1);
            i++; // skip
        }
    }
    return args;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // 从所有插件注册所有 MIME 类型
    for (const ExtensionSystem::PluginSpec *plugin : ExtensionSystem::PluginManager::plugins()) {
        if (!plugin->isEffectivelyEnabled())
            continue;
        const QJsonObject metaData = plugin->metaData();
        const QJsonValue mimetypes = metaData.value("MimeTypes");
        QString strMimeTypes;
        if (Utils::readMultiLineString(mimetypes, &strMimeTypes)) {
            Utils::addMimeTypes(plugin->name() + ".mimetypes", strMimeTypes.trimmed().toUtf8());
        }
    }

    // About theme
    if (ThemeEntry::availableThemes().isEmpty()) {
        *errorString = tr("No themes found in installation.");
        return false;
    }
    const CoreArguments args = parseArguments(arguments);
    Theme *themeFromArg = ThemeEntry::createTheme(args.themeId);
    Theme *theme = themeFromArg ? themeFromArg : ThemeEntry::createTheme(ThemeEntry::themeSetting());
    Theme::setInitialPalette(theme);
    setApplicationTheme(theme);

    m_mainWindow = new MainWindow;
    m_mainWindow->resize(1600, 900);
    m_mainWindow->show();
    return true;
}

void CorePlugin::extensionsInitialized()
{
    if (ExtensionSystem::PluginManager::hasError()) {
        const auto errorOverview = new ExtensionSystem::PluginErrorOverview(m_mainWindow);
        errorOverview->setAttribute(Qt::WA_DeleteOnClose);
        errorOverview->setModal(true);
        errorOverview->show();
    }
    checkSettings();
}

QObject *CorePlugin::remoteCommand(const QStringList & /*options*/,
                                   const QString &workingDirectory,
                                   const QStringList &arguments)
{
    using namespace ExtensionSystem;

    if (!PluginManager::isInitializationDone()) {
        connect(PluginManager::instance(),
                &PluginManager::initializationDone,
                this,
                [this, workingDirectory, arguments]() { remoteCommand(QStringList(), workingDirectory, arguments); });
        return nullptr;
    }
    return nullptr;
}

void CorePlugin::fileOpenRequest(const QString &strFile)
{
    remoteCommand(QStringList(), QString(), QStringList(strFile));
}

void CorePlugin::checkSettings() {}

} // namespace Internal
} // namespace Core
