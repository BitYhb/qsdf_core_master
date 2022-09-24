#ifndef EXTENSIONSYSTEM_PLUGINMANAGER_P_H
#define EXTENSIONSYSTEM_PLUGINMANAGER_P_H

#include "pluginmanager.h"
#include "pluginspec.h"

#include "utils/algorithm.h"

#include <QElapsedTimer>
#include <QReadWriteLock>

#include <queue>

class QTimer;
class QEventLoop;

namespace ExtensionSystem {

class PluginManager;

class PluginManagerPrivate : public QObject
{
    Q_OBJECT
public:
    class TestSpec
    {
    public:
        TestSpec(PluginSpec *pPluginSpec, const QStringList &lstTestFunctionsOrObjects = QStringList())
            : m_pPluginSpec(pPluginSpec)
            , m_lstTestFunctionsOrObjects(lstTestFunctionsOrObjects)
        {}
        PluginSpec *m_pPluginSpec = nullptr;
        QStringList m_lstTestFunctionsOrObjects;
    };

    bool containsTestSpec(PluginSpec *pPluginSpec) const
    {
        return Utils::contains(testSpecs, [pPluginSpec](const TestSpec &ts) { return ts.m_pPluginSpec == pPluginSpec; });
    }

    void removeTestSpec(PluginSpec *pPluginSpec)
    {
        testSpecs = Utils::filtered(testSpecs,
                                    [pPluginSpec](const TestSpec &ts) { return ts.m_pPluginSpec != pPluginSpec; });
    }

    PluginManagerPrivate(PluginManager *pPluginManager);
    ~PluginManagerPrivate();

    void setSettings(Utils::MipsSettings *pSettings);
    void setGlobalSettings(Utils::MipsSettings *pGlobalSettings);

    void addObject(QObject *obj);
    void removeObject(QObject *obj);

    void initProfiling();
    void profilingReport(const char *what, const PluginSpec *spec = nullptr);
    void profilingSummary() const;

    void loadPlugins();
    void loadPlugin(PluginSpec *spce, PluginSpec::State destState);
    void stopAll();
    void deleteAll();
    QVector<PluginSpec *> loadQueue();
    void setPluginPaths(const QStringList &lstPluginPaths);
    void readSettings();
    void resolveDependencies();
    void enableDependenciesIndirectly();
    void checkForProblematicPlugins();

    void setupExtraArguments();

    void shutdown();

    PluginSpec *pluginByName(const QString &name);

    QFlags<PluginManager::PluginManagerAttribute> attributeFlags;

    QHash<QString, QVector<PluginSpec *>> m_hashPluginCategories;
    QVector<PluginSpec *> pluginSpecs;
    std::vector<TestSpec> testSpecs;
    QString m_strPluginIID;
    QVector<QObject *> allObjects;
    QStringList m_lstPluginPaths;
    QStringList m_lstDefaultDisabledPlugins;
    QStringList m_lstDefaultEnabledPlugins;
    QStringList m_lstDisabledPlugins;
    QStringList m_lstForceEnabledPlugins;

    QTimer *delayedInitializeTimer = nullptr;
    std::queue<PluginSpec *> delayedInitializeQueue;

    // ansynchronous shutdown
    QSet<PluginSpec *> asynchronousPlugins;
    QEventLoop *shutdownEventLoop = nullptr;

    QStringList arguments;
    QStringList argumentsForRestart;

    Utils::MipsSettings *m_settings = nullptr;
    Utils::MipsSettings *m_globalSettings = nullptr;

    QScopedPointer<QElapsedTimer> m_profileTimer;
    int m_profileElapsedMS = 0;
    unsigned m_profilingVerbosity = 0;
    QHash<const PluginSpec *, int> m_profileTotal;

    mutable QReadWriteLock m_lock;

    bool m_isInitializationDone = false;
    bool m_bEnableCrashCheck = true;

    PluginManager::ProcessData m_processData;

private:
    PluginManager *q;

    void nextDelayedInitialize();
    void asyncShutdownFinished();

    void readPluginPaths();
    bool loadQueue(PluginSpec *pSpec, QVector<PluginSpec *> &queue, QVector<PluginSpec *> &circularityCheckQueue);
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINMANAGER_P_H
