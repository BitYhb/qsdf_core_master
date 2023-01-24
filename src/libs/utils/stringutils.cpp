#include "stringutils.h"

#include <utils/assert.h>

#include <QJsonArray>
#include <QJsonValue>

namespace Utils {

QSDF_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out)
{
    QSDF_ASSERT(out, return false);
    if (value.isString()) {
        *out = value.toString();
    } else if (value.isArray()) {
        QJsonArray array = value.toArray();
        QStringList lines;
        for (const QJsonValue &line : array) {
            if (!line.isString())
                return false;
            lines.append(line.toString());
        }
        *out = lines.join(QLatin1Char('\n'));
    } else {
        return false;
    }
    return true;
}

} // namespace Utils
