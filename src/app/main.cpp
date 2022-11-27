#include <app/app_version.h>

#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>

#include <utils/mipssettings.h>
#include <utils/quickapplication.h>
#include <utils/stylehelper.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QThreadPool>
#include <QTranslator>

using namespace ExtensionSystem;

enum { OptionIndent = 4, DescriptionIndent = 34 };

constexpr char CORE_PLUGIN_NAME_C[] = "Core";
constexpr char FIXED_OPTIONS_C[]
    = " [OPTION]... \n"
      "Options:\n"
      "    -help                         Display this help\n"
      "    -version                      Display program version\n"
      "    -client                       Attempt to connect to already running first instance\n"
      "    -settings-path <path>         Override the default path where user settings are stored\n"
      "    -install-settings-path <path> Override the default path from where user-independent settings are read\n"
      "    -pid <pid>                    Attempt to connect to instance given by pid\n"
      "    -block                        Block until editor is closed\n"
      "    -plugin-path <path>           Add a custom search path for plugins\n"
      "    -education                    Start clinical education software\n"
      "    -run-target <target={abdomen,chest}>\n"
      "                                  Select the product you want to start\n"
      "    -generate-dev-target <target={plugin,tool}>\n"
      "                                  Generate the corresponding development release suite\n";

constexpr char HELP_OPTION[] = "-help";
constexpr char VERSION_OPTION[] = "-version";
constexpr char CLIENT_OPTION[] = "-client";
constexpr char SETTINGS_OPTION[] = "-settings-path";
constexpr char INSTALL_SETTINGS_OPTIONS[] = "-install-settings-path";
constexpr char PID_OPTION[] = "-pid";
constexpr char BLOCK_OPTION[] = "-block";
constexpr char PLUGIN_PATH_OPTION[] = "-plugin-path";

static QString toHtml(const QString &t)
{
    QString res = t;
    res.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    res.replace(QLatin1Char('<'), QLatin1String("&lt;"));
    res.replace(QLatin1Char('>'), QLatin1String("&gt;"));
    res.insert(0, QLatin1String("<html><pre>"));
    res.append(QLatin1String("</pre></html>"));
    return res;
}

static void displayHelpText(const QString &t)
{
    if (quickApp)
        QMessageBox::information(nullptr, QLatin1String(Core::Constants::QSDF_DISPLAY_NAME), toHtml(t));
    else
        qWarning("%s", qPrintable(t));
}

static void displayError(const QString &t)
{
    qDebug() << t;
}

static QString msgCoreLoadFailure(const QString &why)
{
    return Utils::QuickApplication::translate("Application", "Failed to load core: %1").arg(why);
}

static void printHelp(const QString &name)
{
    QString help;
    QTextStream str(&help);
    str << "Usage: " << name << FIXED_OPTIONS_C;
    PluginManager::formatOptions(str, OptionIndent, DescriptionIndent);
    displayHelpText(help);
}

static void printVersion(const PluginSpec *corePlugin)
{
    Q_UNUSED(corePlugin)
}

static QStringList getPluginPaths()
{
    QStringList lstPluginPaths(QDir::cleanPath(QApplication::applicationDirPath() + '/' + RELATIVE_PLUGIN_PATH));

    QString strPluginPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    strPluginPath += QLatin1Char('/') + Core::Constants::QSDF_SETTINGSVARIANT_STR + QLatin1Char('/');
    strPluginPath += Core::Constants::QSDF_ID;
    strPluginPath += QLatin1String("/plugins/");
    lstPluginPaths.push_back(strPluginPath);
    return lstPluginPaths;
}

QString applicationDirPath(const char *arg = nullptr)
{
    static QString strDir;

    if (arg)
        strDir = QFileInfo(QString::fromLocal8Bit(arg)).dir().absolutePath();

    if (QCoreApplication::instance())
        return QApplication::applicationDirPath();

    return strDir;
}

static QString resourcePath()
{
    return QDir::cleanPath(applicationDirPath() + "/" + RELATIVE_DATA_PATH);
}

static void setupInstallSettings(QString &strInstallSettingsPath)
{
    if (!strInstallSettingsPath.isEmpty() && !QFileInfo(strInstallSettingsPath).isDir()) {
        const QString strErrorMessage
            = QString("-install-settings-path \"%0\" needs to be the path where a %1/%2.ini exist.")
                  .arg(strInstallSettingsPath,
                       QLatin1String(Core::Constants::QSDF_SETTINGSVARIANT_STR),
                       QLatin1String(Core::Constants::QSDF_CASED_ID));
        QMessageBox::critical(nullptr, QLatin1String(Core::Constants::QSDF_DISPLAY_NAME), strErrorMessage);
        strInstallSettingsPath.clear();
    }

    static constexpr char kInstallSettingsKey[] = "Settings/InstallSettings";
    QSettings::setPath(QSettings::IniFormat,
                       QSettings::SystemScope,
                       strInstallSettingsPath.isEmpty() ? resourcePath() : strInstallSettingsPath);

    if (const QSettings installSettings(QSettings::IniFormat,
                                        QSettings::UserScope,
                                        QLatin1String(Core::Constants::QSDF_SETTINGSVARIANT_STR),
                                        QLatin1String(Core::Constants::QSDF_CASED_ID));
        installSettings.contains(kInstallSettingsKey)) {
        QString installSettingsPath = installSettings.value(kInstallSettingsKey).toString();
        if (QDir::isRelativePath(installSettingsPath))
            installSettingsPath = applicationDirPath() + "/" + installSettingsPath;
        QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, installSettingsPath);
    }
}

static Utils::MipsSettings *createUserSettings()
{
    return new Utils::MipsSettings(QSettings::IniFormat,
                                   QSettings::UserScope,
                                   QLatin1String(Core::Constants::QSDF_SETTINGSVARIANT_STR),
                                   QLatin1String(Core::Constants::QSDF_CASED_ID));
}

static Utils::MipsSettings *createSystemSettings()
{
    return new Utils::MipsSettings(QSettings::IniFormat,
                                   QSettings::SystemScope,
                                   QLatin1String(Core::Constants::QSDF_SETTINGSVARIANT_STR),
                                   QLatin1String(Core::Constants::QSDF_CASED_ID));
}

void loadFontFamilyFromFiles()
{
    // share dir
    const QDir fontDir(resourcePath() + "/fonts/");
    const QFileInfoList lstFonts = fontDir.entryInfoList(QStringList{"*.ttf", "*.ttc", "*.otf"}, QDir::Files);
    for (const QFileInfo &fileInfo : lstFonts) {
        QFontDatabase::addApplicationFont(fileInfo.absoluteFilePath());
    }

    // build in
    const QFileInfoList buildInFonts = Utils::StyleHelper::builtInFonts();
    for (const QFileInfo &buildInFont : buildInFonts) {
        QFontDatabase::addApplicationFont(buildInFont.absoluteFilePath());
    }
}

struct Options
{
    QString strPatientId;
    QString strCaseId;
    QString strSettingsPath;
    QString strInstallSettingsPath;
    QStringList lstCustomPluginPaths;
    // list of arguments that were handled and not passed to the application or plugin manager
    QStringList lstPreAppArguments;
    // list of arguments to be passed to the application or plugin manager
    std::vector<char *> appArguments;
};

Options parseCommandLine(int argc, char *argv[])
{
    Options options;
    auto it = argv;
    const auto end = argv + argc;
    while (it != end) {
        const auto strArg = QString::fromLocal8Bit(*it);
        const bool bHasNext = it + 1 != end;
        const auto strNextArg = bHasNext ? QString::fromLocal8Bit(*(it + 1)) : QString();

        if (strArg == SETTINGS_OPTION && bHasNext) {
            ++it;
            options.strSettingsPath = QDir::fromNativeSeparators(strNextArg);
            options.lstPreAppArguments << strArg << strNextArg;
        } else if (strArg == PLUGIN_PATH_OPTION && bHasNext) {
            ++it;
            options.lstCustomPluginPaths += QDir::fromNativeSeparators(strNextArg);
            options.lstPreAppArguments << strArg << strNextArg;
        } else if (strArg == INSTALL_SETTINGS_OPTIONS && bHasNext) {
            ++it;
            options.strInstallSettingsPath = QDir::fromNativeSeparators(strNextArg);
            options.lstPreAppArguments << strArg << strNextArg;
        } else { // arguments that are still passed on to the application
            options.appArguments.push_back(*it);
        }

        ++it;
    }
    return options;
}

class Restarter
{
public:
    Restarter(const int argc, char *argv[])
    {
        Q_UNUSED(argc)
        m_executable = QString::fromLocal8Bit(argv[0]);
        m_workingPath = QDir::currentPath();
    }

    void setArguments(const QStringList &args) { m_args = args; }

    [[nodiscard]] QString executable() const { return m_executable; }
    [[nodiscard]] QStringList arguments() const { return m_args; }
    [[nodiscard]] QString workingPath() const { return m_workingPath; }

    [[nodiscard]] int restartOrExit(const int exitCode) const
    {
        return quickApp->property("restart").toBool() ? restart(exitCode) : exitCode;
    }

    [[nodiscard]] int restart(const int exitCode) const
    {
        QProcess::startDetached(m_executable, m_args, m_workingPath);
        return exitCode;
    }

private:
    QString m_executable;
    QStringList m_args;
    QString m_workingPath;
};

/**
 * Even the best make mistakes sometimes.
 * 人非圣贤，孰能无过。
 */
int main(int argc, char **argv)
{
    Restarter restarter(argc, argv);

    // Manually determine various command line options
    Options options = parseCommandLine(argc, argv);
    applicationDirPath(argv[0]);

    // set logging category message pattern
    qputenv("QT_MESSAGE_PATTERN",
            "[%{time yyyyMMdd h:mm:ss.zzz} "
            "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] "
            "%{file}:%{line} - %{message}");

    qputenv("QSG_RHI_BACKEND", "opengl");
    Utils::QuickApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
    Utils::QuickApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    int nNumberOfArguments = static_cast<int>(options.appArguments.size());

    Utils::QuickApplication app(QLatin1String(Core::Constants::QSDF_DISPLAY_NAME),
                                nNumberOfArguments,
                                options.appArguments.data());
    Utils::QuickApplication::setApplicationName(Core::Constants::QSDF_CASED_ID);
    Utils::QuickApplication::setApplicationVersion(QLatin1String(Core::Constants::QSDF_VERSION_LONG));
    Utils::QuickApplication::setOrganizationName(QLatin1String(Core::Constants::QSDF_SETTINGSVARIANT_STR));
    Utils::QuickApplication::setApplicationDisplayName(Core::Constants::QSDF_DISPLAY_NAME);

    loadFontFamilyFromFiles();
    Utils::QuickApplication::setFont(QFont("PingFang SC"));

    // Initialize global settings and reset-up install settings with QApplication::applicationDirPath
    setupInstallSettings(options.strInstallSettingsPath);
    Utils::MipsSettings *settings = createUserSettings();
    Utils::MipsSettings *globalSettings = createSystemSettings();

    const int nThreadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, 2 * nThreadCount));

    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("org.quick.plugin"));
    PluginManager::setSettings(settings);
    PluginManager::setGlobalSettings(globalSettings);

    // translator
    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages = QLocale::system().uiLanguages();
    if (QString overrideLanguage = settings->value(QLatin1String("General/OverrideLanguage")).toString();
        !overrideLanguage.isEmpty()) {
        uiLanguages.prepend(overrideLanguage);
    }

    const QString &applicationTrPath = resourcePath() + "/translations";
    for (QString locale : qAsConst(uiLanguages)) {
        locale = QLocale(locale).name();
        if (translator.load("quick_" + locale, applicationTrPath)) {
            const QString &qtTrPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, applicationTrPath)) {
                Utils::QuickApplication::installTranslator(&translator);
                Utils::QuickApplication::installTranslator(&qtTranslator);
                break;
            }
        }
    }

    // Load plugins
    const QStringList lstPluginArguments = Utils::QuickApplication::arguments();
    const QStringList lstPluginPaths = getPluginPaths() + options.lstCustomPluginPaths;
    PluginManager::setPluginPaths(lstPluginPaths);
    QMap<QString, QString> foundAppOptions;
    if (lstPluginArguments.size() > 1) {
        QMap<QString, bool> mapAppOptions;
        mapAppOptions.insert(QLatin1String(HELP_OPTION), false);
        mapAppOptions.insert(QLatin1String(VERSION_OPTION), false);
        mapAppOptions.insert(QLatin1String(CLIENT_OPTION), false);
        mapAppOptions.insert(QLatin1String(PID_OPTION), true);
        mapAppOptions.insert(QLatin1String(BLOCK_OPTION), false);
        QString strErrorMessage;
        if (!PluginManager::parseOptions(lstPluginArguments, mapAppOptions, &foundAppOptions, &strErrorMessage)) {
            displayError(strErrorMessage);
            printHelp(QFileInfo(Utils::QuickApplication::applicationFilePath()).baseName());
            return -1;
        }
    }

    // Setup restarter arguments
    restarter.setArguments(options.lstPreAppArguments + PluginManager::argumentsForRestart());
    const QString strSettingsPath = options.lstPreAppArguments.contains(QLatin1String(SETTINGS_OPTION))
                                        ? QString()
                                        : options.strSettingsPath;
    const PluginManager::ProcessData processData = {restarter.executable(),
                                                    options.lstPreAppArguments + PluginManager::argumentsForRestart(),
                                                    restarter.workingPath(),
                                                    strSettingsPath};
    PluginManager::setApplicationProcessData(processData);

    // check plugin for Core
    const auto plugins = PluginManager::plugins();
    PluginSpec *coreplugin = nullptr;
    for (PluginSpec *pSpec : plugins) {
        if (pSpec->name() == CORE_PLUGIN_NAME_C) {
            coreplugin = pSpec;
            break;
        }
    }
    if (nullptr == coreplugin) {
        QString nativePaths = QDir::toNativeSeparators(lstPluginPaths.join(","));
        const QString reason = Utils::QuickApplication::translate("QuickApplication", "Could not find Core plugin in %1")
                                   .arg(nativePaths);
        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (!coreplugin->isEffectivelyEnabled()) {
        const QString reason = Utils::QuickApplication::translate("QuickApplication", "Core plugin is disabled.");
        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (coreplugin->hasError()) {
        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

    if (foundAppOptions.contains(QLatin1String(VERSION_OPTION))) {
        printVersion(coreplugin);
        return 0;
    }

    if (foundAppOptions.contains(QLatin1String(HELP_OPTION))) {
        printHelp(QFileInfo(Utils::QuickApplication::applicationFilePath()).baseName());
        return 0;
    }

    PluginManager::checkForProblematicPlugins();
    PluginManager::loadPlugins();
    if (coreplugin->hasError()) {
        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

    QObject::connect(&app, &Utils::QuickApplication::messageReceived, &pluginManager, &PluginManager::remoteArguments);
    QObject::connect(&app, SIGNAL(fileOpenRequest(QString)), coreplugin->plugin(), SLOT(fileOpenRequest(QString)));

    // shutdown plugin manager on the exit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &pluginManager, &PluginManager::shutdown);

    return restarter.restartOrExit(Utils::QuickApplication::exec());
}
