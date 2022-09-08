#ifndef EXTENSIONSYSTEM_PLUGINMANAGER_H
#define EXTENSIONSYSTEM_PLUGINMANAGER_H

#include "extensionsystem_global.h"

#include <utils/mipssettings.h>

#include <QObject>

namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate;

class MIPS_EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    PluginManager();
    ~PluginManager() override;

    enum class PluginManagerAttribute { LM_UNABRIDGED, LM_ABRIDGED };
    Q_ENUM(PluginManagerAttribute)
    Q_DECLARE_FLAGS(PluginManagerAttributeFlag, PluginManagerAttribute)

    static PluginManager *instance();

    static void setAttribute(PluginManagerAttribute attribute, bool on = true);
    static bool testAttribute(PluginManagerAttribute attribute);

    static void addObject(QObject *obj);
    static void removeObject(QObject *obj);
    static QVector<QObject *> allObjects();

    static void setPluginIID(const QString &strPluginIID);
    static QString pluginIID();

    static void setSettings(Utils::MipsSettings *pSettings);
    static Utils::MipsSettings *settings();
    static void setGlobalSettings(Utils::MipsSettings *pGlobalSettings);
    static Utils::MipsSettings *globalSettings();
    static void writeSettings();

    // 设置插件路径, 搜索插件的所有指定路径及其子目录树
    static void setPluginPaths(const QStringList &lstPluginPaths);
    static QStringList pluginPaths();
    static void loadPlugins();
    // This list is valid directly after the setPluginPaths() call
    static const QVector<PluginSpec *> plugins();
    // Returns a list of plugins in load order
    static QVector<PluginSpec *> loadQueue();
    static void checkForProblematicPlugins();

    // 如果任何插件有错误, 即使插件为启用的, 都将返回true
    static bool hasError();
    static const QStringList allErrors();

    // 返回被插件 pSpec 依赖的插件
    static QSet<PluginSpec *> pluginsRequiredByPlugin(PluginSpec *pSpec);
    // 返回依赖于 pSpec 插件的插件
    static QSet<PluginSpec *> pluginsRequiringPlugin(PluginSpec *pSpec);

    // 解析参数
    static QStringList arguments();
    static QStringList argumentsForRestart();
    static bool parseOptions(const QStringList &lstArgs,
                             const QMap<QString, bool> &mapAppOptions,
                             QMap<QString, QString> *pFoundOptions,
                             QString *pErrorMessage);
    static void formatOptions(QTextStream &str, int optionIndentation, int descriptionIndentation);

    struct ProcessData
    {
        QString m_strExecutable;
        QStringList m_lstArguments;
        QString m_strWorkingPath;
        QString m_strSettingsPath;
    };

    static void setApplicationProcessData(const ProcessData &processData);
    static ProcessData applicationProcessData();

    static void profilingReport(const char *what, const PluginSpec *spec = nullptr);

    // 解析serializedArgument中编码的选项，并将它们与参数一起传递给各自的插件。
    static void remoteArguments(const QString &serializedArgument, QObject *socket);
    static void shutdown();

    static QString systemInformation();

    static bool isInitializationDone();

signals:
    void pluginsChanged();
    void initializationDone();

    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

private:
    friend class PluginManagerPrivate;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINMANAGER_H
