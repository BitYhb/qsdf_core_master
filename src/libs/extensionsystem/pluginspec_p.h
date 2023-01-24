#ifndef EXTENSIONSYSTEM_PLUGINSPEC_P_H
#define EXTENSIONSYSTEM_PLUGINSPEC_P_H

#include "iplugin.h"
#include "pluginspec.h"

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QLoggingCategory>

namespace ExtensionSystem {

Q_DECLARE_LOGGING_CATEGORY(lcPlugin)

class IPlugin;

class PluginSpecPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(PluginSpec)

public:
    PluginSpecPrivate(PluginSpec &object);

    bool read(const QString &strFileName);
    bool provides(const QString &strPluginName, const QString &strVersion) const;
    bool resolveDependencies(const QVector<PluginSpec *>& vPluginSpecs);
    bool loadLibrary();
    bool initializePlugin();
    bool initializeExtensions();
    bool delayedInitialize();
    IPlugin::ShutdownFlag stop();
    void kill();

    void setEnabledByDefault(bool bValue);
    void setEnabledBySettings(bool bValue);

    QPluginLoader m_loader;

    QString name;
    QString version;
    QString compatVersion;
    QString category;
    QVector<PluginDependency> dependencies;
    QHash<PluginDependency, PluginSpec *> dependencySpecs;
    QJsonObject metaData;

    QString m_strPluginLocation;
    QString m_strPluginPath;

    QStringList arguments;

    bool m_bEnabledByDefault = true;
    bool m_bEnabledBySettings = true;
    bool m_bForceEnabled = false;
    bool m_bForceDisabled = false;
    bool m_bEnabledIndirectly = false;
    bool m_bRequired = false;

    IPlugin *plugin = nullptr;

    // State
    PluginSpec::State state = PluginSpec::Invalid;
    bool hasError = false;
    QString errorString;

    static int versionCompare(const QString &strVersion1, const QString &strVersion2);

    QVector<PluginSpec *> enableDependenciesIndirectly(bool bEnableTestDependencies = false);

    bool readMetaData(const QJsonObject &jsonMetaData);

private:
    PluginSpec *q_ptr;

    static const QRegularExpression &versionRegExp();
    bool reportError(const QString &strError);
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINSPEC_P_H
