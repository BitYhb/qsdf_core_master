#include "pluginspec.h"

#include "iplugin.h"
#include "pluginmanager.h"
#include "pluginspec_p.h"

#include <utils/algorithm.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QLoggingCategory>

namespace ExtensionSystem {

Q_LOGGING_CATEGORY(lcPlugin, "logging.extensionsystem")

size_t qHash(const PluginDependency &value)
{
    return qHash(value.m_strName);
}

bool PluginDependency::operator==(const PluginDependency &other) const
{
    return m_strName == other.m_strName && m_strVersion == other.m_strVersion && m_eType == other.m_eType;
}

static QString typeString(PluginDependency::Type type)
{
    switch (type) {
    case ExtensionSystem::PluginDependency::Optional:
        return QString("optional");
    case ExtensionSystem::PluginDependency::Required:
    default:
        return QString();
    }
}

QString PluginDependency::toString() const
{
    return m_strName + " (" + m_strVersion + typeString(m_eType) + ")";
}

namespace {
const char PLUGIN_METADATA[] = "MetaData";
const char PLUGIN_NAME[] = "Name";
const char PLUGIN_VERSION[] = "Version";
const char PLUGIN_REQUIRED[] = "Required";
} // namespace

PluginSpecPrivate::PluginSpecPrivate(PluginSpec &object)
    : q_ptr(&object)
{}

bool PluginSpecPrivate::read(const QString &strFileName)
{
    qCDebug(lcPlugin) << "Reading meta data of" << strFileName;

    // 初始化信息
    state = PluginSpec::Invalid;
    hasError = false;
    errorString.clear();
    dependencies.clear();

    QFileInfo fileInfo(strFileName);
    m_strPluginLocation = fileInfo.absolutePath();
    m_strPluginPath = fileInfo.absoluteFilePath();
    m_loader.setFileName(m_strPluginPath);
    if (m_loader.fileName().isEmpty()) {
        qCDebug(lcPlugin) << "Cannot open file";
        return false;
    }

    if (!readMetaData(m_loader.metaData())) {
        return false;
    }

    state = PluginSpec::Read;
    return true;
}

bool PluginSpecPrivate::provides(const QString &strPluginName, const QString &strVersion) const
{
    if (QString::compare(strPluginName, name, Qt::CaseInsensitive) != 0)
        return false;
    return (versionCompare(version, strVersion) >= 0) && (versionCompare(compatVersion, strVersion) <= 0);
}

bool PluginSpecPrivate::resolveDependencies(const QVector<PluginSpec *>& vPluginSpecs)
{
    if (hasError)
        return false;
    if (state == PluginSpec::Resolved)
        state = PluginSpec::Read; // Go back, re-resolve the dependencies.
    if (state != PluginSpec::Read) {
        errorString = QCoreApplication::translate("PluginSpec", "Resolving dependencies failed because state != Read");
        hasError = true;
        return false;
    }

    QHash<PluginDependency, PluginSpec *> hashResolvedDependencies;
    for (const PluginDependency &dependency : qAsConst(dependencies)) {
        PluginSpec *const pFond = Utils::findOrDefault(vPluginSpecs, [&dependency](PluginSpec *pSpec) {
            return pSpec->provides(dependency.m_strName, dependency.m_strVersion);
        });
        if (pFond == nullptr) {
            if (dependency.m_eType == PluginDependency::Required) {
                hasError = true;
                if (!errorString.isEmpty())
                    errorString.append(QLatin1Char('\n'));
                errorString.append(QCoreApplication::translate("PluginSpec", "Could not resolve dependency '%1(%2)'")
                                       .arg(dependency.m_strName, dependency.m_strVersion));
            }
            continue;
        }
        hashResolvedDependencies.insert(dependency, pFond);
    }

    if (hasError)
        return false;

    dependencySpecs = hashResolvedDependencies;
    state = PluginSpec::Resolved;

    return true;
}

bool PluginSpecPrivate::loadLibrary()
{
    if (hasError)
        return false;

    if (state != PluginSpec::Resolved) {
        if (state == PluginSpec::Loaded)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Library load error (state != Resolved)");
        hasError = true;
        return false;
    }

    if (!m_loader.load()) {
        hasError = true;
        errorString = QDir::toNativeSeparators(m_strPluginPath) + QString::fromLatin1(": ") + m_loader.errorString();
        return false;
    }
    auto *pluginObject = qobject_cast<IPlugin *>(m_loader.instance());
    if (nullptr == pluginObject) {
        hasError = true;
        errorString = QCoreApplication::translate("PluginSpec", "Library load error (cannot derive from IPlugin)");
        m_loader.unload();
        return false;
    }

    state = PluginSpec::Loaded;
    plugin = pluginObject;
    plugin->setPluginSpec(q_ptr);
    return true;
}

bool PluginSpecPrivate::initializePlugin()
{
    if (hasError)
        return false;

    if (state != PluginSpec::Loaded) {
        if (state == PluginSpec::Initialized)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialize error (state != Loaded)");
        hasError = true;
        return false;
    }

    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialize error (no plugin instance)");
        hasError = true;
        return false;
    }

    QString strErrorMsg;
    if (!plugin->initialize(arguments, &strErrorMsg)) {
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialize error (%1)").arg(strErrorMsg);
        hasError = true;
        return false;
    }

    state = PluginSpec::Initialized;
    return true;
}

bool PluginSpecPrivate::initializeExtensions()
{
    if (hasError)
        return false;

    if (state != PluginSpec::Initialized) {
        if (state == PluginSpec::Running)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "extensions initialize error (state != Initialized)");
        hasError = true;
        return false;
    }

    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "extensions initialize error (no plugin instance)");
        hasError = true;
        return false;
    }

    plugin->extensionsInitialized();
    state = PluginSpec::Running;
    return true;
}

bool PluginSpecPrivate::delayedInitialize()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Running)
        return false;
    if (!plugin) {
        errorString
            = QCoreApplication::translate("PluginSpec",
                                          "Internal error: have no plugin instance to perform delayedInitialize");
        hasError = true;
        return false;
    }
    return plugin->delayedInitialize();
}

IPlugin::ShutdownFlag PluginSpecPrivate::stop()
{
    if (!plugin)
        return IPlugin::ShutdownFlag::SynchronousShutdown;

    state = PluginSpec::Stopped;
    return plugin->aboutToShutdown();
}

void PluginSpecPrivate::kill()
{
    if (plugin) {
        delete plugin;
        plugin = nullptr;
        state = PluginSpec::Deleted;
    }
}

void PluginSpecPrivate::setEnabledByDefault(bool bValue)
{
    m_bEnabledByDefault = bValue;
}

void PluginSpecPrivate::setEnabledBySettings(bool bValue)
{
    m_bEnabledBySettings = bValue;
}

int PluginSpecPrivate::versionCompare(const QString &strVersion1, const QString &strVersion2)
{
    const auto match1 = versionRegExp().match(strVersion1);
    const auto match2 = versionRegExp().match(strVersion2);
    if (!match1.hasMatch() || !match2.hasMatch())
        return 0;
    for (int i = 0; i < 4; ++i) {
        const int nNumber1 = match1.captured(i + 1).toInt();
        const int nNumber2 = match2.captured(i + 1).toInt();
        if (nNumber1 < nNumber2)
            return -1;
        if (nNumber1 > nNumber2)
            return 1;
    }
    return 0;
}

QVector<PluginSpec *> PluginSpecPrivate::enableDependenciesIndirectly(bool bEnableTestDependencies)
{
    if (!q_ptr->isEffectivelyEnabled()) // plugin not enabled, nothing to do
        return {};
    QVector<PluginSpec *> vEnabled;
    for (auto it = dependencySpecs.cbegin(), end = dependencySpecs.cend(); it != end; ++it) {
        if (it.key().m_eType == PluginDependency::Required
            && (!bEnableTestDependencies || it.key().m_eType == PluginDependency::Test))
            continue;
        PluginSpec *pDependencySpec = it.value();
        if (pDependencySpec->isEffectivelyEnabled()) {
            pDependencySpec->d_ptr->m_bEnabledIndirectly = true;
            vEnabled << pDependencySpec;
        }
    }
    return vEnabled;
}

static inline QString metaValueMissing(const char *key)
{
    return QCoreApplication::translate("pluginSpec", "Metadata \"%1\" is missing").arg(key);
}

static inline QString metaValueIsNotString(const char *key)
{
    return QCoreApplication::translate("pluginSpec", "Metadata \"%1\" is not a string").arg(key);
}

static inline QString metaValueIsNotBool(const char *key)
{
    return QCoreApplication::translate("pluginSpec", "Metadata \"%1\" is not a bool").arg(key);
}

bool PluginSpecPrivate::readMetaData(const QJsonObject &jsonMetaData)
{
    qCDebug(lcPlugin) << "plugin meta data:" << QJsonDocument(jsonMetaData).toJson(QJsonDocument::Compact);
    QJsonValue value;
    value = jsonMetaData.value(QLatin1String("IID"));
    if (!value.isString()) {
        qCDebug(lcPlugin) << "Not a plugin (string IID not found)";
        return false;
    }
    if (value.toString() != PluginManager::pluginIID()) {
        qCDebug(lcPlugin) << "Plugin ignored (IID mismatch)";
        return false;
    }

    value = jsonMetaData.value(QLatin1String(PLUGIN_METADATA));
    if (!value.isObject())
        return reportError(tr("Plugin meta data not found"));
    metaData = value.toObject();

    value = metaData.value(QLatin1String(PLUGIN_NAME));
    if (value.isUndefined())
        return reportError(metaValueMissing(PLUGIN_NAME));
    if (!value.isString())
        return reportError(metaValueIsNotString(PLUGIN_NAME));
    name = value.toString();

    value = metaData.value(QLatin1String(PLUGIN_VERSION));
    if (value.isUndefined())
        return reportError(metaValueMissing(PLUGIN_VERSION));
    if (!value.isString())
        return reportError(metaValueIsNotString(PLUGIN_VERSION));
    version = value.toString();

    value = metaData.value(QLatin1String(PLUGIN_REQUIRED));
    if (!value.isUndefined() && !value.isBool())
        return reportError(metaValueIsNotBool(PLUGIN_REQUIRED));
    m_bRequired = value.toBool(false);

    return true;
}

const QRegularExpression &PluginSpecPrivate::versionRegExp()
{
    static const QRegularExpression reg("^([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?$");
    return reg;
}

bool PluginSpecPrivate::reportError(const QString &strError)
{
    errorString = strError;
    hasError = true;
    return true;
}

PluginSpec::PluginSpec(QObject *parent)
    : QObject(parent)
    , d_ptr(new PluginSpecPrivate(*this))
{}

PluginSpec::~PluginSpec() = default;

PluginSpec *PluginSpec::read(const QString &strFilePath)
{
    auto spec = new PluginSpec;
    if (!spec->d_ptr->read(strFilePath)) {
        delete spec;
        return nullptr;
    }
    return spec;
}

QString PluginSpec::name() const
{
    Q_D(const PluginSpec);
    return d->name;
}

QString PluginSpec::version() const
{
    Q_D(const PluginSpec);
    return d->version;
}

QString PluginSpec::compatVersion() const
{
    Q_D(const PluginSpec);
    return d->compatVersion;
}

QString PluginSpec::category() const
{
    Q_D(const PluginSpec);
    return d->category;
}

QJsonObject PluginSpec::metaData() const
{
    Q_D(const PluginSpec);
    return d->metaData;
}

QString PluginSpec::pluginLocation() const
{
    Q_D(const PluginSpec);
    return d->m_strPluginLocation;
}

QString PluginSpec::pluginPath() const
{
    Q_D(const PluginSpec);
    return d->m_strPluginPath;
}

QStringList PluginSpec::arguments() const
{
    Q_D(const PluginSpec);
    return d->arguments;
}

void PluginSpec::setArguments(const QStringList &arguments)
{
    Q_D(PluginSpec);
    d->arguments = arguments;
}

void PluginSpec::addArgument(const QString &argument)
{
    Q_D(PluginSpec);
    d->arguments.push_back(argument);
}

void PluginSpec::addArguments(const QStringList &arguments)
{
    Q_D(PluginSpec);
    d->arguments.append(arguments);
}

bool PluginSpec::isEffectivelyEnabled() const
{
    if (isForceEnabled() || isEnabledIndirectly())
        return true;
    if (isForceDisabled())
        return false;
    return isEnabledBySettings();
}

bool PluginSpec::isEnabledByDefault() const
{
    Q_D(const PluginSpec);
    return d->m_bEnabledByDefault;
}
bool PluginSpec::isEnabledBySettings() const
{
    Q_D(const PluginSpec);
    return d->m_bEnabledBySettings;
}

bool PluginSpec::isForceEnabled() const
{
    Q_D(const PluginSpec);
    return d->m_bForceEnabled;
}

bool PluginSpec::isForceDisabled() const
{
    Q_D(const PluginSpec);
    return d->m_bForceDisabled;
}

bool PluginSpec::isEnabledIndirectly() const
{
    Q_D(const PluginSpec);
    return d->m_bEnabledIndirectly;
}

bool PluginSpec::isRequired() const
{
    Q_D(const PluginSpec);
    return d->m_bRequired;
}

QHash<PluginDependency, PluginSpec *> PluginSpec::dependencySpecs() const
{
    Q_D(const PluginSpec);
    return d->dependencySpecs;
}

bool PluginSpec::requiresAny(const QSet<PluginSpec *> plugins) const
{
    Q_D(const PluginSpec);
    return Utils::anyOf(d->dependencySpecs.keys(), [d, &plugins](const PluginDependency &pluginDep) {
        return pluginDep.m_eType == PluginDependency::Required && plugins.contains(d->dependencySpecs.value(pluginDep));
    });
}

IPlugin *PluginSpec::plugin() const
{
    Q_D(const PluginSpec);
    return d->plugin;
}

PluginSpec::State PluginSpec::state() const
{
    Q_D(const PluginSpec);
    return d->state;
}

bool PluginSpec::hasError() const
{
    Q_D(const PluginSpec);
    return d->hasError;
}

QString PluginSpec::errorString() const
{
    Q_D(const PluginSpec);
    return d->errorString;
}

bool PluginSpec::provides(const QString &strPluginName, const QString &strVersion) const
{
    Q_D(const PluginSpec);
    return d->provides(strPluginName, strVersion);
}

} // namespace ExtensionSystem
