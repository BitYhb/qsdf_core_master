#include "stringutils.h"

#include <utils/mipsassert.h>

#include <QJsonArray>
#include <QJsonValue>

namespace Utils {

QUICK_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out)
{
    QUICK_ASSERT(out, return false);
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
