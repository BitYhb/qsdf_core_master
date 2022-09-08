#ifndef UTILS_STRINGUTILS_H
#define UTILS_STRINGUTILS_H

#include "utils_global.h"

QT_BEGIN_NAMESPACE
class QJsonValue;
QT_END_NAMESPACE

namespace Utils {

MIPS_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out);

}

#endif // UTILS_STRINGUTILS_H
