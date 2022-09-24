#pragma once

#include <extensionsystem/iplugin.h>

namespace Core {

class ActionManager;

namespace Internal {

class MainQmlApplicationEngine;

class CorePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.MIPS.MIPSoftwarePlugin" FILE "Core.json")
public:
    CorePlugin();
    ~CorePlugin() override;

    static CorePlugin *instance();

    bool initialize(const QStringList &arguments, QString *errorString) override;

    void extensionsInitialized() override;

    QObject *remoteCommand(const QStringList &options,
                           const QString &workingDirectory,
                           const QStringList &arguments) override;

public slots:
    void fileOpenRequest(const QString &strFile);

private:
    void checkSettings();

    ActionManager *m_actionManager = nullptr;
    MainQmlApplicationEngine *m_mainQmlEngine = nullptr;
};

} // namespace Internal
} // namespace Core

