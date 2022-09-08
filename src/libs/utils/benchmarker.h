#ifndef UTILS_BENCHMARKER_H
#define UTILS_BENCHMARKER_H

#include "utils_global.h"

#include <QElapsedTimer>
#include <QString>

QT_BEGIN_NAMESPACE
class QLoggingCategory;
QT_END_NAMESPACE

namespace Utils {

class MIPS_UTILS_EXPORT Benchmarker
{
public:
    static void report(const QString &testsuite, const QString &testcase, qint64 ms, const QString &tags = QString());
    static void report(const QLoggingCategory &cat,
                       const QString &testsuite,
                       const QString &testcase,
                       qint64 ms,
                       const QString &tags = QString());
};

} // namespace Utils

#endif // UTILS_BENCHMARKER_H
