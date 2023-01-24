#ifndef QUICK_IPLUGIN_H
#define QUICK_IPLUGIN_H

#include "extensionsystem_export.h"

#include <QObject>

namespace ExtensionSystem {

class PluginSpecPrivate;
class PluginSpec;
class IPluginPrivate;

class QSDF_EXTENSIONSYSTEM_EXPORT IPlugin : public QObject
{
    Q_OBJECT
public:
    enum class ShutdownFlag {
        SynchronousShutdown,
        AsynchronousShutdown,
    };

    explicit IPlugin(QObject *parent = nullptr);
    ~IPlugin() override;

    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual void extensionsInitialized() {}
    virtual bool delayedInitialize() { return false; }
    virtual ShutdownFlag aboutToShutdown() { return ShutdownFlag::SynchronousShutdown; }
    virtual QObject *remoteCommand(const QStringList &options,
                                   const QString &workingDirectory,
                                   const QStringList &arguments)
    {
        return nullptr;
    }

    void setPluginSpec(PluginSpec *pluginSpec);
    [[nodiscard]] PluginSpec *pluginSpec() const;

signals:
    void asynchronousShutdownFinished();

protected:
    QScopedPointer<IPluginPrivate> d_ptr;

private:
    friend class PluginSpecPrivate;

    Q_DECLARE_PRIVATE(IPlugin)
    Q_DISABLE_COPY(IPlugin)
};

} // namespace ExtensionSystem

#endif // QUICK_IPLUGIN_H
