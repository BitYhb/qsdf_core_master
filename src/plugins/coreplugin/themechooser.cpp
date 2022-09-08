#include "themechooser.h"
#include "coreconstants.h"
#include "icore.h"

#include <utils/algorithm.h>

#include <utility>

using namespace Utils;

namespace Core {
namespace Internal {

class ThemeChooserPrivate
{
public:
    ThemeChooserPrivate(QWidget *widget);
    ~ThemeChooserPrivate();

public:
};

ThemeChooserPrivate::ThemeChooserPrivate(QWidget *widget) {}

ThemeChooserPrivate::~ThemeChooserPrivate() {}

static void addThemesFromPath(const QString &path, QList<ThemeEntry> *themes)
{
    static const QLatin1String extension("*.mipstheme");
    QDir themeDir(path);
    themeDir.setNameFilters({extension});
    themeDir.setFilter(QDir::Files);
    const QStringList themeList = themeDir.entryList();
    foreach (const QString &fileName, themeList) {
        QString id = QFileInfo(fileName).completeBaseName();
        themes->append(ThemeEntry(Id::fromString(id), themeDir.absoluteFilePath(fileName)));
    }
}

Internal::ThemeEntry::ThemeEntry(Utils::Id id, QString filePath)
    : m_id(id)
    , m_filePath(std::move(filePath))
{}

Utils::Id Internal::ThemeEntry::id() const
{
    return m_id;
}

QString ThemeEntry::filePath() const
{
    return m_filePath;
}

QList<ThemeEntry> ThemeEntry::availableThemes()
{
    QList<ThemeEntry> themes;

    static const QFileInfo installThemeDir = ICore::resourcePath("themes");
    static const QFileInfo userThemeDir = ICore::userResourcePath("themes");
    addThemesFromPath(installThemeDir.absoluteFilePath(), &themes);
    if (themes.isEmpty())
        qWarning() << "Warning: No themes found in installation:" << installThemeDir.absoluteFilePath();

    int defaultIndex = Utils::indexOf(themes, Utils::equal(&ThemeEntry::id, Id(Constants::DEFAULT_THEME)));
    if (defaultIndex > 0) {
        ThemeEntry defaultEntry = themes.takeAt(defaultIndex);
        themes.prepend(defaultEntry);
    }
    addThemesFromPath(userThemeDir.absoluteFilePath(), &themes);
    return themes;
}

static QString defaultThemeId()
{
    // TODO:
    // return Theme::systemUsesDarkMode() ? QString(Constants::DEFAULT_DARK_THEME) : QString(Constants::DEFAULT_THEME);
    return QString(Constants::DEFAULT_DARK_THEME);
}

Id ThemeEntry::themeSetting()
{
    const Id setting = Id::fromSetting(ICore::settings()->value(Constants::SETTINGS_THEME, defaultThemeId()));

    const QList<ThemeEntry> themes = availableThemes();
    if (themes.empty()) {
        return Id();
    }
    const bool settingValid = Utils::contains(themes, Utils::equal(&ThemeEntry::id, setting));

    return settingValid ? setting : themes.first().id();
}

Theme *ThemeEntry::createTheme(Id id)
{
    if (!id.isValid())
        return nullptr;

    const ThemeEntry entry = Utils::findOrDefault(availableThemes(), Utils::equal(&ThemeEntry::id, id));
    if (!entry.id().isValid())
        return nullptr;

    QSettings themeSettings(entry.filePath(), QSettings::IniFormat);
    Theme *theme = new Theme(entry.id().toString());
    theme->readSettings(&themeSettings);

    // Set the current icon theme
    QIcon::setThemeName(theme->displayName().toLower());

    return theme;
}

ThemeChooser::ThemeChooser(QWidget *parent)
    : QWidget(parent)
{
    d = new ThemeChooserPrivate(this);
}

ThemeChooser::~ThemeChooser()
{
    delete d;
}

void ThemeChooser::apply() {}

} // namespace Internal
} // namespace Core
