#include "theme_p.h"

#include <utils/assert.h>

#include <QApplication>
#include <QMetaEnum>
#include <QSettings>

namespace Utils {

static Theme *m_applicationTheme = nullptr;

ThemePrivate::ThemePrivate()
{
    const QMetaObject &metaObject = Theme::staticMetaObject;
    colors.resize(metaObject.enumerator(metaObject.indexOfEnumerator("Color")).keyCount());
    flags.resize(metaObject.enumerator(metaObject.indexOfEnumerator("Flag")).keyCount());
    fontSizes.resize(metaObject.enumerator(metaObject.indexOfEnumerator("FontSize")).keyCount());
}

void setThemeApplicationPalette()
{
    if (m_applicationTheme && m_applicationTheme->flag(Theme::Flag::ApplyThemePaletteGlobally))
        QApplication::setPalette(m_applicationTheme->palette());
}

void setApplicationTheme(Theme *theme)
{
    if (m_applicationTheme == theme)
        return;
    delete m_applicationTheme;
    m_applicationTheme = theme;

    setThemeApplicationPalette();
}

Theme::Theme(const QString &id, QObject *parent /* = nullptr */)
    : QObject(parent)
    , d(new ThemePrivate)
{
    d->id = id;
}

Theme::Theme(Theme *originTheme, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d(new ThemePrivate(*originTheme->d))
{}

Theme::~Theme()
{
    delete d;
}

QColor Theme::color(Theme::Color role) const
{
    return d->colors[(int) role].first;
}

bool Theme::flag(Theme::Flag flag) const
{
    return d->flags[(int) flag];
}

int Theme::fontSize(Theme::FontSize size) const
{
    return d->fontSizes[(int) size];
}

QPalette Theme::palette() const
{
    QPalette pal = initialPalette();
    return pal;
}

QStringList Theme::preferredStyles() const
{
    return d->preferredStyles;
}

QString Theme::id() const
{
    return d->id;
}

QString Theme::filePath() const
{
    return d->fileName;
}

QString Theme::displayName() const
{
    return d->displayName;
}

void Theme::setDisplayName(const QString &displayName)
{
    d->displayName = displayName;
}

void Theme::readSettings(QSettings *settings)
{
    d->fileName = settings->fileName();
    const QMetaObject &mo = *metaObject();

    // General
    {
        d->displayName = settings->value(QLatin1String("ThemeName"), QLatin1String("unnamed")).toString();
        d->preferredStyles = settings->value(QLatin1String("PreferredStyles")).toStringList();
        d->preferredStyles.removeAll(QString());
    }
    // Color
    {
        settings->beginGroup(QLatin1String("Colors"));
        QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("Color"));
        for (int i = 0, total = me.keyCount(); i < total; ++i) {
            const QString key = QLatin1String(me.key(i));
            if (!settings->contains(key)) {
                qWarning("Theme \"%s\" does not set the color for key \"%s\"", qPrintable(d->fileName), qPrintable(key));
                continue;
            }
            d->colors[i] = readNamedColor(settings->value(key).toString());
        }
        settings->endGroup();
    }

    // Flag
    {
        settings->beginGroup(QLatin1String("Flags"));
        QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("Flag"));
        for (int i = 0, total = me.keyCount(); i < total; ++i) {
            const QString key = QLatin1String(me.key(i));
            QSDF_ASSERT(settings->contains(key), return );
            d->flags[i] = settings->value(key).toBool();
        }
        settings->endGroup();
    }

    // FontSize
    {
        settings->beginGroup(QLatin1String("FontSizes"));
        QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("FontSize"));
        for (int i = 0, total = me.keyCount(); i < total; ++i) {
            const QString key = QLatin1String(me.key(i));
            QSDF_ASSERT(settings->contains(key), return);
            d->fontSizes[i] = settings->value(key).toInt();
        }
        settings->endGroup();
    }
}

static QPalette copyPalette(const QPalette &palette)
{
    QPalette respal;
    for (int group = 0; group < QPalette::NColorGroups; ++group) {
        for (int role = 0; role < QPalette::NColorRoles; ++role) {
            respal.setBrush(QPalette::ColorGroup(group),
                            QPalette::ColorRole(role),
                            palette.brush(QPalette::ColorGroup(group), QPalette::ColorRole(role)));
        }
    }
    return respal;
}

QPalette Theme::initialPalette()
{
    static QPalette palette = copyPalette(QApplication::palette());
    return palette;
}

void Theme::setInitialPalette(Theme *initTheme)
{
    Q_UNUSED(initTheme);
    initialPalette();
}

QPair<QColor, QString> Theme::readNamedColor(const QString &color)
{
    if (d->palette.contains(color))
        return qMakePair(d->palette[color], color);

    const QColor col("#" + color);
    if (!col.isValid()) {
        qWarning("Color \"%s\" is not a valid color", qPrintable(color));
        return qMakePair(Qt::black, QString());
    }
    return qMakePair(col, QString());
}

Theme *applicationTheme()
{
    return m_applicationTheme;
}

Theme *proxyTheme()
{
    return new Theme(m_applicationTheme);
}

} // namespace Utils
