#pragma once

#include "utils_export.h"

#include <QFileInfo>
#include <QFont>

namespace Utils {

class MIPS_UTILS_EXPORT StyleHelper
{
public:
    // 针对某个图片, 根据 devicePixelRatio() 返回该图片可用的有效分辨率
    static QList<int> effectiveImageResolution(const QString &fileName);

    // 通过传入原始的图像文件和指定 dpr, 返回对应分辨率的图像
    static QString imageFileWithResolution(const QString &fileName, int dpr);

    // 获取所有内置字体
    static QFileInfoList builtInFonts();
};

} // namespace Utils
