#ifndef UTILS_MIPSSETTINGS_H
#define UTILS_MIPSSETTINGS_H

#include "utils_export.h"

#include <QSettings>

namespace Utils {

class QUICK_UTILS_EXPORT MipsSettings : public QSettings
{
    Q_OBJECT

public:
    using QSettings::QSettings;

    template<typename T>
    void setValueWithDefault(const QString &strKey, const T &value, const T &defaultValue);
    template<typename T>
    void setValueWithDefault(const QString &strKey, const T &value);

    template<typename T>
    static void setValueWithDefault(QSettings *pSettings, const QString &strKey, const T &value, const T &defaultValue);
    template<typename T>
    static void setValueWithDefault(QSettings *pSettings, const QString &strKey, const T &value);
};

template<typename T>
void MipsSettings::setValueWithDefault(const QString &strKey, const T &value, const T &defaultValue)
{
    setValueWithDefault(this, strKey, value, defaultValue);
}

template<typename T>
void MipsSettings::setValueWithDefault(const QString &strKey, const T &value)
{
    setValueWithDefault(this, strKey, value);
}

template<typename T>
void MipsSettings::setValueWithDefault(QSettings *pSettings,
                                       const QString &strKey,
                                       const T &value,
                                       const T &defaultValue)
{
    if (value == defaultValue)
        pSettings->remove(strKey);
    else
        pSettings->setValue(strKey, QVariant::fromValue(value));
}

template<typename T>
void MipsSettings::setValueWithDefault(QSettings *pSettings, const QString &strKey, const T &value)
{
    if (value == T())
        pSettings->remove(strKey);
    else
        pSettings->setValue(strKey, QVariant::fromValue(value));
}

} // namespace Utils

#endif // UTILS_MIPSSETTINGS_H
