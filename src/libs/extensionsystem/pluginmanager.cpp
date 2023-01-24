#include "pluginmanager.h"

#include "optionsparser.h"
#include "pluginmanager_p.h"
#include "pluginspec_p.h"

#include <utils/algorithm.h>
#include <utils/benchmarker.h>
#include <utils/mimetypes.h>

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QLibrary>
#include <QMetaEnum>
#include <QTimer>
#include <QWriteLocker>

const char S_IGNORED_PLUGINS[] = "Plugins/Ignored";
const char S_FORCEENABLED_PLUGINS[] = "Plugins/ForceEnabled";
const int DELAYED_INITIALIZE_INTERVAL = 20; // ms

using namespace Utils;

namespace ExtensionSystem {

class LockFile
{
public:
    static QString filePath(PluginManagerPrivate *pPm)
    {
        return QFileInfo(pPm->m_settings->fileName()).absolutePath() + '/' + QCoreApplication::applicationName() + '.'
               + QCryptographicHash::hash(QCoreApplication::applicationDirPath().toUtf8(), QCryptographicHash::Sha1)
                     .left(8)
                     .toHex()
               + ".lockfile";
    }

    static std::optional<QString> lockedPluginName(PluginManagerPrivate *pPm)
    {
        const QString strLockFilePath = LockFile::filePath(pPm);
        if (QFile::exists(strLockFilePath)) {
            QFile lockFile(strLockFilePath);
            if (lockFile.open(QFile::ReadOnly)) {
                const auto pluginName = QString::fromUtf8(lockFile.readLine()).trimmed();
                lockFile.close();
                return pluginName;
            } else {
                qCDebug(lcPlugin) << "Lockfile" << strLockFilePath << "exists but does not have read permission";
            }
        }

        return {};
    }

    LockFile(PluginManagerPrivate *pPm, PluginSpec *pSpec)
        : m_strFilePath(LockFile::filePath(pPm))
    {
        QDir().mkpath(QFileInfo(m_strFilePath).absolutePath());
        QFile lockFile(m_strFilePath);
        if (lockFile.open(QFile::WriteOnly)) {
            lockFile.write(pSpec->name().toUtf8());
            lockFile.write("\n");
            lockFile.close();
        } else {
            qCDebug(lcPlugin) << "Unable to write to lock file" << m_strFilePath;
        }
    }

    ~LockFile() { QFile::remove(m_strFilePath); }

private:
    QString m_strFilePath;
};

static PluginManagerPrivate *d = nullptr;
static PluginManager *m_instance = nullptr;

/*!
 * \brief Construct a default PluginManager instance object
 * \warning Creating multiple PluginManager instances may lead to unexpected results.
 * \see instance()
 */
PluginManager::PluginManager()
{
    m_instance = this;
    d = new PluginManagerPrivate(this);
}

PluginManager::~PluginManager()
{
    delete d;
    d = nullptr;
}

/*!
 * \brief Get the instantiated object of the singleton PluginManager
 * \return
 */
PluginManager *PluginManager::instance()
{
    return m_instance;
}

void PluginManager::setAttribute(PluginManagerAttribute attribute, bool on)
{
    if (d->attributeFlags.testFlag(attribute) == on)
        return;

    d->attributeFlags.setFlag(attribute, on);
}

bool PluginManager::testAttribute(PluginManagerAttribute attribute)
{
    return d->attributeFlags.testFlag(attribute);
}

void PluginManager::addObject(QObject *obj)
{
    d->addObject(obj);
}

void PluginManager::removeObject(QObject *obj)
{
    d->removeObject(obj);
}

QVector<QObject *> PluginManager::allObjects()
{
    return d->allObjects;
}

void PluginManager::setPluginIID(const QString &strPluginIID)
{
    d->m_strPluginIID = strPluginIID;
}

QString PluginManager::pluginIID()
{
    return d->m_strPluginIID;
}

void PluginManager::setSettings(Utils::MipsSettings *pSettings)
{
    d->setSettings(pSettings);
}

Utils::MipsSettings *PluginManager::settings()
{
    return d->m_settings;
}

void PluginManager::setGlobalSettings(Utils::MipsSettings *pGlobalSettings)
{
    d->setGlobalSettings(pGlobalSettings);
}

Utils::MipsSettings *PluginManager::globalSettings()
{
    return d->m_globalSettings;
}

void PluginManager::writeSettings() {}

void PluginManager::setPluginPaths(const QStringList &lstPluginPaths)
{
    d->setPluginPaths(lstPluginPaths);
}

QStringList PluginManager::pluginPaths()
{
    return d->m_lstPluginPaths;
}

void PluginManager::loadPlugins()
{
    d->loadPlugins();
}

const QVector<PluginSpec *> PluginManager::plugins()
{
    return d->pluginSpecs;
}

QVector<PluginSpec *> PluginManager::loadQueue()
{
    return d->loadQueue();
}

void PluginManager::checkForProblematicPlugins()
{
    d->checkForProblematicPlugins();
}

bool PluginManager::hasError()
{
    return Utils::anyOf(plugins(), [](const PluginSpec *spec) {
        // 只有在启动插件是才显示错误
        return spec->hasError() && spec->isEffectivelyEnabled();
    });
}

const QStringList PluginManager::allErrors()
{
    return Utils::transform<QStringList>(Utils::filtered(plugins(),
                                                         [](const PluginSpec *spec) {
                                                             return spec->hasError() && spec->isEffectivelyEnabled();
                                                         }),
                                         [](const PluginSpec *spec) {
                                             return spec->name().append(": ").append(spec->errorString());
                                         });
}

QSet<PluginSpec *> PluginManager::pluginsRequiredByPlugin(PluginSpec *pSpec)
{
    return {};
}

QSet<PluginSpec *> PluginManager::pluginsRequiringPlugin(PluginSpec *pSpec)
{
    return {};
}

QStringList PluginManager::arguments()
{
    return d->arguments;
}

QStringList PluginManager::argumentsForRestart()
{
    return d->argumentsForRestart;
}

bool PluginManager::parseOptions(const QStringList &lstArgs,
                                 const QMap<QString, bool> &mapAppOptions,
                                 QMap<QString, QString> *pFoundOptions,
                                 QString *pErrorMessage)
{
    OptionsParser options(lstArgs, mapAppOptions, pFoundOptions, pErrorMessage, d);
    return options.parse();
}

static inline void indent(QTextStream &str, int indent)
{
    str << QString(indent, ' ');
}

static inline void formatOption(QTextStream &str,
                                const QString &opt,
                                const QString &parm,
                                const QString &description,
                                int optionIndentation,
                                int descriptionIndentation)
{
    int remainingIndent = descriptionIndentation - optionIndentation - opt.size();
    indent(str, optionIndentation);
    str << opt;
    if (!parm.isEmpty()) {
        str << " <" << parm << '>';
        remainingIndent -= 3 + parm.size();
    }
    if (remainingIndent >= 1) {
        indent(str, remainingIndent);
    } else {
        str << '\n';
        indent(str, descriptionIndentation);
    }
    str << description << '\n';
}

void PluginManager::formatOptions(QTextStream &str, int optionIndentation, int descriptionIndentation)
{
    formatOption(str,
                 QLatin1String(OptionsParser::PROFILE_OPTION),
                 QString(),
                 QLatin1String("Profile plugin loading"),
                 optionIndentation,
                 descriptionIndentation);
    formatOption(str,
                 QLatin1String(OptionsParser::PATIENT_ID_OPTION),
                 QLatin1String("patient-id"),
                 QLatin1String("Specify the Patient ID information for the DICOM to load"),
                 optionIndentation,
                 descriptionIndentation);
    formatOption(str,
                 QLatin1String(OptionsParser::CASE_ID_OPTION),
                 QLatin1String("case-id"),
                 QLatin1String("Specifies the instance of the data to load"),
                 optionIndentation,
                 descriptionIndentation);
}

void PluginManager::setApplicationProcessData(const ProcessData &processData)
{
    d->m_processData = processData;
}

PluginManager::ProcessData PluginManager::applicationProcessData()
{
    return d->m_processData;
}

void PluginManager::profilingReport(const char *what, const PluginSpec *spec)
{
    d->profilingReport(what, spec);
}

void PluginManager::remoteArguments(const QString &serializedArgument, QObject *socket)
{
    if (serializedArgument.isEmpty())
        return;
}

QString PluginManager::systemInformation()
{
    return {};
}

/*!
 * \brief Shuts down and deletes all plugins.
 */
void PluginManager::shutdown()
{
    d->shutdown();
}

bool PluginManager::isInitializationDone()
{
    return d->m_isInitializationDone;
}

PluginManagerPrivate::PluginManagerPrivate(PluginManager *pPluginManager)
    : q(pPluginManager)
{}

PluginManagerPrivate::~PluginManagerPrivate() = default;

void PluginManagerPrivate::setSettings(Utils::MipsSettings *pSettings)
{
    delete m_settings;
    m_settings = pSettings;
    if (m_settings != nullptr)
        m_settings->setParent(this);
}

void PluginManagerPrivate::setGlobalSettings(Utils::MipsSettings *pGlobalSettings)
{
    delete m_globalSettings;
    m_globalSettings = pGlobalSettings;
    if (m_globalSettings != nullptr)
        m_globalSettings->setParent(this);
}

void PluginManagerPrivate::addObject(QObject *obj)
{
    QWriteLocker locker(&m_lock);
    if (nullptr == obj) {
        qWarning() << "PluginManagerPrivate::addObject(): trying to add null object";
        return;
    }
    if (allObjects.contains(obj)) {
        qWarning() << "PluginManagerPrivate::addObject(): trying to add duplicate object";
        return;
    }

    if (m_profilingVerbosity && !m_profileTimer.isNull()) {
        const int absoluteElapsedMS = int(m_profileTimer->elapsed());
        qDebug("  %-43s %8dms", obj->metaObject()->className(), absoluteElapsedMS);
    }

    allObjects.append(obj);
    emit q->objectAdded(obj);
}

void PluginManagerPrivate::removeObject(QObject *obj)
{
    if (nullptr == obj) {
        qWarning() << "PluginManagerPrivate::removeObject(): trying to remove null object";
        return;
    }
    if (!allObjects.contains(obj)) {
        qWarning() << "PluginManagerPrivate::addObject(): object not in list";
        return;
    }

    emit q->aboutToRemoveObject(obj);
    QWriteLocker locker(&m_lock);
    allObjects.removeAll(obj);
}

void PluginManagerPrivate::initProfiling()
{
    if (m_profileTimer.isNull()) {
        m_profileTimer.reset(new QElapsedTimer);
        m_profileTimer->start();
        m_profileElapsedMS = 0;
        qDebug("Profiling started");
    } else {
        m_profilingVerbosity++;
    }
}

void PluginManagerPrivate::profilingReport(const char *what, const PluginSpec *spec)
{
    if (!m_profileTimer.isNull()) {
        const int nAbsoluteElapsedMS = int(m_profileTimer->elapsed());
        const int nElapsedMS = nAbsoluteElapsedMS - m_profileElapsedMS;
        m_profileElapsedMS = nAbsoluteElapsedMS;
        if (spec)
            qDebug("%-22s %-22s %8dms (%8dms)", what, qUtf8Printable(spec->name()), nAbsoluteElapsedMS, nElapsedMS);
        else
            qDebug("%-22s %8dms (%8dms)", what, nAbsoluteElapsedMS, nElapsedMS);
        if (what && *what == '<') {
            QString tc;
            if (spec) {
                m_profileTotal[spec] += nElapsedMS;
                tc = spec->name() + '_';
            }
            tc += QString::fromUtf8((QByteArray(what + 1)));
            Utils::Benchmarker::report("loadPlugins", tc, nElapsedMS);
        }
    }
}

void PluginManagerPrivate::profilingSummary() const
{
    if (m_profileTimer.isNull()) {
        QMultiMap<int, const PluginSpec *> sorter;
        int total = 0;

        auto totalEnd = m_profileTotal.constEnd();
        for (auto it = m_profileTotal.constBegin(); it != totalEnd; ++it) {
            sorter.insert(it.value(), it.key());
            total += it.value();
        }

        auto sorterEnd = sorter.constEnd();
        for (auto it = sorter.constBegin(); it != sorterEnd; ++it)
            qDebug("%-22s %8dms   ( %5.2f%% )", qPrintable(it.value()->name()), it.key(), 100.0 * it.key() / total);
        qDebug("Total: %8dms", total);
        Utils::Benchmarker::report("loadPlugins", "Total", total);
    }
}

void PluginManagerPrivate::loadPlugins()
{
    const QVector<PluginSpec *> queue = loadQueue();
    Utils::setMimeStartupPhase(StartupPhase::PluginsLoading);
    for (PluginSpec *pSpec : queue)
        loadPlugin(pSpec, PluginSpec::Loaded);

    Utils::setMimeStartupPhase(StartupPhase::PluginsInitializing);
    for (PluginSpec *pSpec : queue)
        loadPlugin(pSpec, PluginSpec::Initialized);

    Utils::setMimeStartupPhase(StartupPhase::PluginsDelayedInitializing);
    Utils::reverseForeach(queue, [this](PluginSpec *spec) {
        loadPlugin(spec, PluginSpec::Running);
        if (spec->state() == PluginSpec::Running) {
            delayedInitializeQueue.push(spec);
        } else {
            // Plugin initialization failed, so cleanup after it
            spec->d_ptr->kill();
        }
    });
    emit q->pluginsChanged();
    Utils::setMimeStartupPhase(StartupPhase::UpAndRunning);

    delayedInitializeTimer = new QTimer;
    delayedInitializeTimer->setInterval(DELAYED_INITIALIZE_INTERVAL);
    delayedInitializeTimer->setSingleShot(true);
    connect(delayedInitializeTimer, &QTimer::timeout, this, &PluginManagerPrivate::nextDelayedInitialize);
    delayedInitializeTimer->start();
}

void PluginManagerPrivate::loadPlugin(PluginSpec *spec, PluginSpec::State destState)
{
    if (spec->hasError() || spec->state() != destState - 1)
        return;

    // don't load disabled plugins
    if (!spec->isEffectivelyEnabled() && destState == PluginSpec::Loaded)
        return;

    std::unique_ptr<LockFile> lockFile;
    if (m_bEnableCrashCheck && destState < PluginSpec::Stopped)
        lockFile = std::make_unique<LockFile>(this, spec);

    switch (destState) {
    case PluginSpec::Running:
        profilingReport(">initializeExtensions", spec);
        spec->d_ptr->initializeExtensions();
        profilingReport("<initializeExtensions", spec);
        return;
    case PluginSpec::Deleted:
        profilingReport(">delete", spec);
        spec->d_ptr->kill();
        profilingReport("<delete", spec);
        return;
    default:
        break;
    }

    // check if dependencies have loaded without error
    const QHash<PluginDependency, PluginSpec *> depSpecs = spec->dependencySpecs();
    for (auto it = depSpecs.cbegin(), end = depSpecs.cend(); it != end; ++it) {
        if (it.key().m_eType != PluginDependency::Required)
            continue;
        PluginSpec *depSpec = it.value();
        if (depSpec->state() != destState) {
            spec->d_ptr->hasError = true;
            spec->d_ptr->errorString
                = PluginManager::tr("Plugin failed to load because dependencies failed to load: %1 (%2)\nReason:%3")
                      .arg(depSpec->name(), depSpec->version(), depSpec->errorString());
            return;
        }
    }

    switch (destState) {
    case PluginSpec::Loaded:
        profilingReport(">loadLibrary", spec);
        spec->d_ptr->loadLibrary();
        profilingReport("<loadLibrary", spec);
        break;
    case PluginSpec::Initialized:
        profilingReport(">initializePlugin", spec);
        spec->d_ptr->initializePlugin();
        profilingReport("<initializePlugin", spec);
        break;
    case PluginSpec::Stopped:
        profilingReport(">stop", spec);
        if (spec->d_ptr->stop() == IPlugin::ShutdownFlag::AsynchronousShutdown) {
            asynchronousPlugins << spec;
            connect(spec->plugin(), &IPlugin::asynchronousShutdownFinished, this, [this, spec] {
                asynchronousPlugins.remove(spec);
                if (asynchronousPlugins.isEmpty()) {
                    shutdownEventLoop->exec();
                }
            });
        }
        profilingReport("<stop", spec);
        break;
    default:
        break;
    }
}

void PluginManagerPrivate::stopAll()
{
    if (delayedInitializeTimer && delayedInitializeTimer->isActive()) {
        delayedInitializeTimer->stop();
        delete delayedInitializeTimer;
        delayedInitializeTimer = nullptr;
    }

    const QVector<PluginSpec *> queue = loadQueue();
    for (PluginSpec *spec : queue) {
        loadPlugin(spec, PluginSpec::Stopped);
    }
}

void PluginManagerPrivate::deleteAll()
{
    Utils::reverseForeach(loadQueue(), [this](PluginSpec *spec) { loadPlugin(spec, PluginSpec::Deleted); });
}

QVector<PluginSpec *> PluginManagerPrivate::loadQueue()
{
    QVector<PluginSpec *> resultQueue;
    for (PluginSpec *pSpec : qAsConst(pluginSpecs)) {
        QVector<PluginSpec *> circularityCheckQueue;
        loadQueue(pSpec, resultQueue, circularityCheckQueue);
    }
    return resultQueue;
}

void PluginManagerPrivate::setPluginPaths(const QStringList &lstPluginPaths)
{
    qCDebug(lcPlugin) << "Plugin search paths:" << lstPluginPaths;
    qCDebug(lcPlugin) << "Required IID:" << m_strPluginIID;

    m_lstPluginPaths = lstPluginPaths;
    readSettings();
    readPluginPaths();
    setupExtraArguments();
}

void PluginManagerPrivate::readSettings()
{
    // Default
    if (m_globalSettings != nullptr) {
        m_lstDefaultDisabledPlugins = m_globalSettings->value(S_IGNORED_PLUGINS).toStringList();
        m_lstDefaultEnabledPlugins = m_globalSettings->value(S_FORCEENABLED_PLUGINS).toStringList();
    }

    // User
    if (m_settings != nullptr) {
        m_lstDisabledPlugins = m_settings->value(S_IGNORED_PLUGINS).toStringList();
        m_lstForceEnabledPlugins = m_settings->value(S_FORCEENABLED_PLUGINS).toStringList();
    }
}

void PluginManagerPrivate::resolveDependencies()
{
    for (PluginSpec *pSpec : qAsConst(pluginSpecs)) {
        pSpec->d_ptr->resolveDependencies(pluginSpecs);
    }
}

void PluginManagerPrivate::enableDependenciesIndirectly()
{
    for (PluginSpec *pSpec : qAsConst(pluginSpecs)) {
        pSpec->d_ptr->m_bEnabledIndirectly = false;
        // cannot use reverse loadQueue here, because test dependencies can introduce circles
        QVector<PluginSpec *> queue = Utils::filtered(pluginSpecs, &PluginSpec::isEffectivelyEnabled);
        while (!queue.isEmpty()) {
            PluginSpec *pSpec = queue.takeFirst();
            queue += pSpec->d_ptr->enableDependenciesIndirectly(containsTestSpec(pSpec));
        }
    }
}

void PluginManagerPrivate::checkForProblematicPlugins()
{
    if (!m_bEnableCrashCheck)
        return;

    const std::optional<QString> pluginName = LockFile::lockedPluginName(this);
    if (pluginName.has_value()) {
        PluginSpec *pSpec = pluginByName(pluginName.value());
        if (pSpec != nullptr && pSpec->isRequired()) {
            const QSet<PluginSpec *> dependents = PluginManager::pluginsRequiringPlugin(pSpec);
            const auto dependentsNames = Utils::transform<QStringList>(dependents, &PluginSpec::name);
            qCDebug(lcPlugin) << dependentsNames;
        }
    }
}

void PluginManagerPrivate::setupExtraArguments()
{
    const QMetaObject &mo = *q->metaObject();
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("PluginManagerAttribute"));
    for (int i = 0, total = me.keyCount(); i < total; ++i) {
        const int value = me.value(i);
        if (d->attributeFlags.testFlag(PluginManager::PluginManagerAttribute(value))) {
            foreach (PluginSpec *spec, pluginSpecs) {
                spec->addArguments(
                    {QString("-%1").arg(QLatin1String(me.name())).toLower(), QString(me.key(i)).toLower()});
            }
        }
    }
}

void PluginManagerPrivate::shutdown()
{
    stopAll();
    if (!asynchronousPlugins.isEmpty()) {
        shutdownEventLoop = new QEventLoop;
        shutdownEventLoop->exec();
    }
    deleteAll();
    if (!allObjects.isEmpty()) {
        qDebug() << "There are" << allObjects.size() << "objects left in the plugin manager pool.";
        // 故意在这里分割调试信息，因为如果列表中包含已删除的对象，我们至少会得到关于对象数量的信息；
        qDebug() << "The following objects left in the plugin manager pool:" << allObjects;
    }
}

PluginSpec *PluginManagerPrivate::pluginByName(const QString &name)
{
    return Utils::findOrDefault(pluginSpecs, [name](PluginSpec *pSpec) { return name == pSpec->name(); });
}

static const QStringList pluginFiles(const QStringList &lstPluginPaths)
{
    QStringList lstPluginFiles;
    QStringList lstSearchPaths = lstPluginPaths;
    while (!lstSearchPaths.isEmpty()) {
        const QDir dir(lstSearchPaths.takeFirst());
        const QFileInfoList lstFiles = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
        const QStringList lstAbsoluteFilePaths = Utils::transform(lstFiles, &QFileInfo::absoluteFilePath);
        lstPluginFiles += Utils::filtered(lstAbsoluteFilePaths,
                                          [](const QString &strPath) { return QLibrary::isLibrary(strPath); });
        const QFileInfoList lstDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        lstSearchPaths += Utils::transform(lstDirs, &QFileInfo::absoluteFilePath);
    }
    return lstPluginFiles;
}

void PluginManagerPrivate::nextDelayedInitialize()
{
    while (!delayedInitializeQueue.empty()) {
        PluginSpec *spec = delayedInitializeQueue.front();
        delayedInitializeQueue.pop();
        profilingReport(">delayedInitialize", spec);
        bool delay = spec->d_ptr->delayedInitialize();
        profilingReport("<delayedInitialize", spec);
        if (delay) {
            break; // do next delayedInitialize after a delay
        }
    }
    if (delayedInitializeQueue.empty()) {
        m_isInitializationDone = true;
        delete delayedInitializeTimer;
        delayedInitializeTimer = nullptr;
        profilingSummary();
        emit q->initializationDone();
    } else {
        delayedInitializeTimer->start();
    }
}

void PluginManagerPrivate::readPluginPaths()
{
    qDeleteAll(pluginSpecs);
    pluginSpecs.clear();
    m_hashPluginCategories.clear();

    // default
    m_hashPluginCategories.insert(QString(), QVector<PluginSpec *>());

    // Read the plugin spec information
    for (const QString &pluginFile : pluginFiles(m_lstPluginPaths)) {
        PluginSpec *pSpec = PluginSpec::read(pluginFile);
        if (pSpec == nullptr)
            continue;

        if (pSpec->isEnabledByDefault() && m_lstDefaultDisabledPlugins.contains(pSpec->name())) {
            pSpec->d_ptr->setEnabledByDefault(false);
            pSpec->d_ptr->setEnabledBySettings(false);
        } else if (!pSpec->isEnabledByDefault() && m_lstDefaultEnabledPlugins.contains(pSpec->name())) {
            pSpec->d_ptr->setEnabledByDefault(true);
            pSpec->d_ptr->setEnabledBySettings(true);
        }
        if (!pSpec->isEnabledByDefault() && m_lstForceEnabledPlugins.contains(pSpec->name()))
            pSpec->d_ptr->setEnabledBySettings(true);
        if (pSpec->isEnabledByDefault() && m_lstDisabledPlugins.contains(pSpec->name()))
            pSpec->d_ptr->setEnabledBySettings(false);

        m_hashPluginCategories[pSpec->category()].append(pSpec);
        pluginSpecs.append(pSpec);
    }

    resolveDependencies();
    enableDependenciesIndirectly();

    // ensure deterministic plugin load order by sorting
    Utils::sort(pluginSpecs, &PluginSpec::name);
    emit q->pluginsChanged();
}

bool PluginManagerPrivate::loadQueue(PluginSpec *pSpec,
                                     QVector<PluginSpec *> &queue,
                                     QVector<PluginSpec *> &circularityCheckQueue)
{
    if (queue.contains(pSpec))
        return true;

    // 检测循环依赖关系
    if (circularityCheckQueue.contains(pSpec)) {
        pSpec->d_ptr->hasError = true;
        pSpec->d_ptr->errorString = PluginManager::tr("Circular dependency detection:");
        pSpec->d_ptr->errorString += QLatin1Char('\n');
        int nIndex = circularityCheckQueue.indexOf(pSpec);
        for (int i = nIndex; i < circularityCheckQueue.size(); ++i) {
            pSpec->d_ptr->errorString.append(
                PluginManager::tr("%1 (%2) depends on")
                    .arg(circularityCheckQueue.at(i)->name(), circularityCheckQueue.at(i)->version()));
            pSpec->d_ptr->errorString += QLatin1Char('\n');
        }
        pSpec->d_ptr->errorString.append(PluginManager::tr("%1 (%2)").arg(pSpec->name(), pSpec->version()));
        return false;
    }

    // 检查是否有依赖项, 此时状态必须为 PluginSpec::Resolved
    circularityCheckQueue.append(pSpec);
    if (pSpec->d_ptr->state == PluginSpec::Invalid || pSpec->d_ptr->state == PluginSpec::Read) {
        queue.append(pSpec);
        return false;
    }

    const QHash<PluginDependency, PluginSpec *> dependencies = pSpec->dependencySpecs();
    for (auto it = dependencies.cbegin(), end = dependencies.cend(); it != end; ++it) {
        if (it.key().m_eType == PluginDependency::Test)
            continue;
        PluginSpec *depSpec = it.value();
        if (!loadQueue(depSpec, queue, circularityCheckQueue)) {
            pSpec->d_ptr->hasError = true;
            pSpec->d_ptr->errorString
                = PluginManager::tr("Plugin failed to load because dependencies failed to load: %1 (%2)\nReason:%3")
                      .arg(depSpec->name(), depSpec->version(), depSpec->errorString());
            return false;
        }
    }

    queue.append(pSpec);
    return true;
}

} // namespace ExtensionSystem
