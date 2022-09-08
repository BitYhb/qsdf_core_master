#include "iplugin.h"
#include "pluginspec.h"

namespace ExtensionSystem {

class IPluginPrivate
{
    Q_DECLARE_PUBLIC(IPlugin)
public:
    explicit IPluginPrivate(IPlugin &object)
        : q_ptr(&object)
    {}
    PluginSpec *pluginSpec = nullptr;

protected:
    IPlugin *q_ptr = nullptr;
};

/*!
 * \enum IPlugin::ShutdownFlag
 * \brief enum type holds whether the plugin is shut down synchronously or asynchronously.
 * 
 * \var IPlugin::ShutdownFlag::SynchronousShutdown
 * The plugin is shut down synchronously.
 * \var IPlugin::ShutdownFlag::AsynchronousShutdown
 * The plugin needs to perform asynchronous actions before it shuts down.
 */

IPlugin::IPlugin(QObject *parent)
    : QObject(parent)
    , d_ptr(new IPluginPrivate(*this))
{}

IPlugin::~IPlugin() = default;

void IPlugin::setPluginSpec(PluginSpec *pluginSpec)
{
    Q_D(IPlugin);
    d->pluginSpec = pluginSpec;
}

/*!
 * \fn bool ExtensionSystem::IPlugin::initialize(const QStringList &arguments, QString *errorString)
 * \brief Called after the plugin is loaded and an instance of IPlugin is created
 *
 * The initialize function of plugins that depend on this plugin will be called
 * after the initialize function of this plugin is called with \a arguments.
 * Plugins should initialize their internal state in the function.
 *
 * \return Returns whether the initialization was successful. If it does not,
 * errorString should be set to a user-readable message describing the reason.
 *
 * \see extensionsInitialized() and delayedInitialize()
 */

/*!
 * \fn void ExtensionSystem::IPlugin::extensionsInitialized()
 * \brief Called after the initialize() function has been called, and after both
 * the initialize() and \c extensionsInitialized() functions of plugins that depend on
 * this plugin have been called.
 *
 * In this function, the plugin can assume that plugins that depend on this plugin
 * are fully <em>up and running</em>. It is a good place to look in the
 * global object pool for objects that have been provided by weakly dependent plugins.
 *
 * \see initialize() and delayedInitialize()
 */

/*!
 * \fn bool ExtensionSystem::IPlugin::delayedInitialize()
 * \brief Called after all plugin's extensionsInitialized() function has been called,
 * and after the \c delayedInitialize() function of plugins that depend on this plugin
 * has been called.
 *
 * \see initialize() and extensionsInitialized()
 */

/*!
 * \fn ShutdownFlag ExtensionSystem::IPlugin::aboutToShutdown()
 * \brief Called during a shutdown sequence in the same order as initialization
 * before the plugins get deleted in reverse order.
 *
 * \see asynchronousShutdownFinished()
 */

PluginSpec *IPlugin::pluginSpec() const
{
    Q_D(const IPlugin);
    return d->pluginSpec;
}

} // namespace ExtensionSystem
