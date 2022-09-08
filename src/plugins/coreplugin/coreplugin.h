#ifndef CORE_COREPLUGIN_H
#define CORE_COREPLUGIN_H

#include "core_global.h"

#include <extensionsystem/iplugin.h>

namespace Core {

class ActionManager;

namespace Internal {

class MainWindow;

class CorePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.truesight.MIPS.MIPSoftwarePlugin" FILE "Core.json")
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
    MainWindow *m_mainWindow = nullptr;
};

} // namespace Internal
} // namespace Core

#endif // CORE_COREPLUGIN_H
