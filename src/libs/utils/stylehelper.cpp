#include "stylehelper.h"

#include <QApplication>
#include <QDirIterator>
#include <QFile>

namespace Utils {

QList<int> StyleHelper::effectiveImageResolution(const QString &fileName)
{
    QList<int> result;
    const int maxResolution = qApp->devicePixelRatio();
    for (int dpr = 1; dpr <= maxResolution; ++dpr) {
        if (QFile::exists(imageFileWithResolution(fileName, dpr)))
            result.append(dpr);
    }
    return result;
}

QString StyleHelper::imageFileWithResolution(const QString &fileName, int dpr)
{
    return {};
}

static inline bool isFontFile(const QFileInfo &fileInfo)
{
    static const QStringList fontSuffixes({"ttf", "otf"});
    const QString suffix = fileInfo.suffix();
    return fontSuffixes.contains(suffix, Qt::CaseInsensitive);
}

QFileInfoList StyleHelper::builtInFonts()
{
    QFileInfoList result;
    QDirIterator it(QLatin1String(":/utils"), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QFileInfo fileInfo = it.nextFileInfo();
        if (fileInfo.isFile() && isFontFile(fileInfo)) {
            result << fileInfo;
        }
    }
    return result;
}

} // namespace Utils
