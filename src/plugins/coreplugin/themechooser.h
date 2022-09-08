#ifndef CORE_THEMECHOOSER_H
#define CORE_THEMECHOOSER_H

#include <utils/id.h>
#include <utils/theme/theme.h>

#include <QWidget>

namespace Core {
namespace Internal {

class ThemeChooserPrivate;

class ThemeEntry
{
public:
    ThemeEntry() = default;
    ThemeEntry(Utils::Id id, QString filePath);

    Utils::Id id() const;
    QString filePath() const;

    static QList<ThemeEntry> availableThemes();
    static Utils::Id themeSetting();
    static Utils::Theme *createTheme(Utils::Id id);

private:
    Utils::Id m_id;
    QString m_filePath;
    mutable QString m_displayName;
};

class ThemeChooser : public QWidget
{
public:
    explicit ThemeChooser(QWidget *parent = nullptr);
    ~ThemeChooser() override;

    void apply();

private:
    ThemeChooserPrivate *d;
};

} // namespace Internal
} // namespace Core

#endif // CORE_THEMECHOOSER_H
