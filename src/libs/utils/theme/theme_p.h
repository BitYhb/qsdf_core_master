#ifndef UTILS_THEME_P_H
#define UTILS_THEME_P_H

#include "utils_export.h"
#include "theme.h"

#include <QString>
#include <QMap>

namespace Utils {

class QSDF_UTILS_EXPORT ThemePrivate
{
public:
    ThemePrivate();

    QString id;
    QString fileName;
    QString displayName;
    QStringList preferredStyles;
    QVector<QPair<QColor, QString>> colors;
    QVector<bool> flags;
    QVector<int> fontSizes;
    QMap<QString, QColor> palette;
};

QSDF_UTILS_EXPORT void setApplicationTheme(Theme *theme);
QSDF_UTILS_EXPORT void setThemeApplicationPalette();

} // namespace Utils

#endif // UTILS_THEME_P_H
