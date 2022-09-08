#ifndef PLUGINERROROVERVIEW_H
#define PLUGINERROROVERVIEW_H

#include "extensionsystem_global.h"

#include <QDialog>

namespace ExtensionSystem {

class MIPS_EXTENSIONSYSTEM_EXPORT PluginErrorOverview : public QDialog
{
public:
    explicit PluginErrorOverview(QWidget *parent = nullptr);
    ~PluginErrorOverview() override;
};

} // namespace ExtensionSystem

#endif // PLUGINERROROVERVIEW_H
