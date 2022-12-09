#include "coreplugin.h"
#include "qmlapplicationengine.h"
#include "themechooser.h"

#include <coreplugin/corepluginmacro.h>
#include <coreplugin/framelessquickwindow.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <utils/mimetypes.h>
#include <utils/quickapplication.h>
#include <utils/stringutils.h>
#include <utils/theme/theme.h>
#include <utils/theme/theme_p.h>

#include <QJsonObject>
#include <QMetaEnum>

namespace Core::Internal {

static CorePlugin *m_instance = nullptr;

CorePlugin::CorePlugin()
{
    m_instance = this;
}

CorePlugin::~CorePlugin()
{
    Utils::setApplicationTheme(nullptr);
}

CorePlugin *CorePlugin::instance()
{
    return m_instance;
}

struct CoreArguments
{
    QColor overrideColor;
    Utils::Id themeId;
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
    const QMetaObject metaObject = PluginManager::staticMetaObject;
    const QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("PluginManagerAttribute"));
    for (int i = 0; i < arguments.size(); ++i) {
        if (arguments.at(i) == QLatin1String("-plugin-manager-attribute")) {
            static const QString abridged = QLatin1String(metaEnum.valueToKey(static_cast<int>(PMAttr::LM_ABRIDGED)));
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
    const auto themeFromArg = ThemeEntry::createTheme(args.themeId);
    const auto theme = themeFromArg ? themeFromArg : ThemeEntry::createTheme(ThemeEntry::themeSetting());
    Utils::Theme::setInitialPalette(theme);
    setApplicationTheme(theme);

    qmlRegisterType<FramelessQuickWindow>("qsdf.gui.window", 1, 0, "FramelessQuickWindow");

    m_mainQmlEngine = new QmlApplicationEngine;
    const QUrl url(u"qrc:/main.qml"_qs);
    connect(
        m_mainQmlEngine,
        &QmlApplicationEngine::objectCreated,
        quickApp,
        [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                qCInfo(Core::lcCorePlugin, "Application failed to load, exit(-1)");
                Utils::CustomEventApplication::publishEvent("QuitApplication", Qt::AutoConnection, -1);
            }
        },
        Qt::QueuedConnection);
    m_mainQmlEngine->load(url);
    return true;
}

void CorePlugin::extensionsInitialized()
{
    if (ExtensionSystem::PluginManager::hasError()) {
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

} // namespace Core::Internal
