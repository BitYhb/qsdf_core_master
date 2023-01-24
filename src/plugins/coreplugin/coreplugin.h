#pragma once

#include <extensionsystem/iplugin.h>

namespace Core {

class ActionManager;

namespace Internal {

class QmlApplicationEngine;

class CorePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qsdf-core.plugin" FILE "Core.json")
public:
    CorePlugin();
    ~CorePlugin() override;

    static CorePlugin *instance();

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    QObject *remoteCommand(const QStringList &options,
                           const QString &workingDirectory,
                           const QStringList &arguments) override;

    [[nodiscard]] std::shared_ptr<QmlApplicationEngine> qmlEngine() const;
    [[nodiscard]] QString domainViewerURI();

public slots:
    void fileOpenRequest(const QString &strFile);

private:
    void checkSettings();

    ActionManager *m_actionManager = nullptr;
    std::shared_ptr<QmlApplicationEngine> m_qmlEngine;
    QString m_domainViewerURI;
};

} // namespace Internal
} // namespace Core
