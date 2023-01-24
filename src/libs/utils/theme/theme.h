#ifndef UTILS_THEME_H
#define UTILS_THEME_H

#include "utils_export.h"

#include <QObject>
#include <QPalette>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Utils {

class ThemePrivate;

class QSDF_UTILS_EXPORT Theme : public QObject
{
    Q_OBJECT
public:
    Theme(const QString &id, QObject *parent = nullptr);
    ~Theme() override;

    enum class Color {
        /* Application */
        ApplicationBackgroundColor,
        ApplicationTitleCopyrightTextColor,
        ApplicationTitleTextColor,
        PatientInformationCardBackgroundColor,
        SplitterColor,
        SystemToolbarBackgroundColor,
        TitleBarBackgroundColor,
        UniversalToolbarBackgroundColor,
        UniversalToolBarBorderColor,
        ToolsPanelWidgetBackgroundColor,

        /* Icons */
        IconsDisableColor,
        IconsMaskColor
    };
    enum class Flag { ApplyThemePaletteGlobally };
    enum class FontSize { H1, H2, H3, H4, H5 };
    Q_ENUM(Color)
    Q_ENUM(Flag)
    Q_ENUM(FontSize)

    QColor color(Theme::Color role) const;
    bool flag(Theme::Flag flag) const;
    int fontSize(Theme::FontSize size) const;
    QPalette palette() const;
    QStringList preferredStyles() const;

    QString id() const;
    QString filePath() const;
    QString displayName() const;
    void setDisplayName(const QString &displayName);

    void readSettings(QSettings *settings);

    static QPalette initialPalette();
    static void setInitialPalette(Theme *initTheme);

protected:
    Theme(Theme *originTheme, QObject *parent = nullptr);
    ThemePrivate *d;

private:
    friend QSDF_UTILS_EXPORT Theme *applicationTheme();
    friend QSDF_UTILS_EXPORT Theme *proxyTheme();
    QPair<QColor, QString> readNamedColor(const QString &color);
};

QSDF_UTILS_EXPORT Theme *applicationTheme();
QSDF_UTILS_EXPORT Theme *proxyTheme();

} // namespace Utils

#endif // UTILS_THEME_H
