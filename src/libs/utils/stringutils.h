#pragma once
#include "utils_export.h"

#include <QJsonValue>

namespace Utils {

QSDF_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out);

}
