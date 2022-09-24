#ifndef UTILS_STRINGUTILS_H
#define UTILS_STRINGUTILS_H

#include "utils_export.h"

#include <QJsonValue>

namespace Utils {

MIPS_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out);

}

#endif // UTILS_STRINGUTILS_H
