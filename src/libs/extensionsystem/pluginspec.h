#ifndef EXTENSIONSYSTEM_PLUGINSPEC_H
#define EXTENSIONSYSTEM_PLUGINSPEC_H

#include "extensionsystem_export.h"

#include <QScopedPointer>
#include <QObject>

namespace ExtensionSystem {

class PluginSpecPrivate;
class IPlugin;

struct QSDF_EXTENSIONSYSTEM_EXPORT PluginDependency
{
    enum Type { Required, Optional, Test };

    PluginDependency()
        : m_eType(Required)
    {}

    friend size_t qHash(const PluginDependency &value);

    QString m_strName;
    QString m_strVersion;
    Type m_eType;

    bool operator==(const PluginDependency &other) const;
    QString toString() const;
};

class QSDF_EXTENSIONSYSTEM_EXPORT PluginSpec : public QObject
{
    Q_OBJECT
public:
    enum State { Invalid, Read, Resolved, Loaded, Initialized, Running, Stopped, Deleted };

    explicit PluginSpec(QObject *parent = nullptr);
    ~PluginSpec() override;

    static PluginSpec *read(const QString &strFilePath);

    QString name() const;
    QString version() const;
    QString compatVersion() const;
    QString category() const;
    QJsonObject metaData() const;

    QString pluginLocation() const;
    QString pluginPath() const;

    QStringList arguments() const;
    void setArguments(const QStringList &arguments);
    void addArgument(const QString &argument);
    void addArguments(const QStringList &arguments);

    // Returns whether the plugin is loaded at startup.
    bool isEffectivelyEnabled() const;
    bool isEnabledByDefault() const;
    bool isEnabledBySettings() const;
    bool isForceEnabled() const;
    bool isForceDisabled() const;
    bool isEnabledIndirectly() const;
    bool isRequired() const;

    // 返回现有插件的依赖项列表，如果状态达到PluginSpec::Resolved则有效
    QHash<PluginDependency, PluginSpec *> dependencySpecs() const;
    bool requiresAny(const QSet<PluginSpec *> plugins) const;

    // linked plugin instance, valid after 'Loaded' state is reached
    IPlugin *plugin() const;

    // State
    PluginSpec::State state() const;
    bool hasError() const;
    QString errorString() const;

    // Returns whether this plugin can be used to fill in a dependency of the given pluginName and version.
    bool provides(const QString &strPluginName, const QString &strVersion) const;

protected:
    QScopedPointer<PluginSpecPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_DISABLE_COPY(PluginSpec)
    friend class PluginManagerPrivate;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINSPEC_H
