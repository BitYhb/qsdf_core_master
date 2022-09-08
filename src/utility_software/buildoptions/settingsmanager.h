#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>

class SettingsManager : public QSettings
{
    explicit SettingsManager(QObject *parent = nullptr);
public:
};
Q_GLOBAL_STATIC(SettingsManager, settingsManager)

#endif // SETTINGSMANAGER_H
