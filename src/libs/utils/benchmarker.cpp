#include "benchmarker.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTimer>

static Q_LOGGING_CATEGORY(benchmarksLog, "logging.benchmark", QtWarningMsg);

namespace Utils {

void Benchmarker::report(const QString &testsuite, const QString &testcase, qint64 ms, const QString &tags)
{
    report(benchmarksLog(), testsuite, testcase, ms, tags);
}

void Benchmarker::report(
    const QLoggingCategory &cat, const QString &testsuite, const QString &testcase, qint64 ms, const QString &tags)
{
    static const QByteArray quitAfter = qgetenv("QSDF_QUIT_AFTER_BENCHMARKER");
    QString t = "unit=ms";
    if (!tags.isEmpty())
        t += "," + tags;

    const QByteArray testSuite = testsuite.toUtf8();
    const QByteArray testCase = testcase.toUtf8();
    qCDebug(cat, "%s::%s: %lld { %s }", testSuite.data(), testCase.data(), ms, t.toUtf8().data());
    if (!quitAfter.isEmpty() && quitAfter == testSuite + "::" + testCase)
        QTimer::singleShot(1000, qApp, &QCoreApplication::quit);
}

} // namespace Utils
