#include "assert.h"

#include <utils/macrodefinition.h>

using namespace Utils;

QSDF_UTILS_EXPORT void Utils::dumpBacktrace(int maxdepth)
{
    if (maxdepth == -1)
        maxdepth = 1000;
    Q_UNUSED(maxdepth);
}

QSDF_UTILS_EXPORT void Utils::writeAssertLocation(const char *msg)
{
    static bool goBoom = qEnvironmentVariableIsSet("QSDF_FATAL_ASSERTS");
    if (goBoom)
        qCCritical(lcUtils, "SOFT ASSERT made fatal: %s", msg);
    else
        qCDebug(lcUtils, "SOFT ASSERT: %s", msg);

    static int backTraceMaxDepthValue = qEnvironmentVariableIntValue("QSDF_BACKTRACE_MAX_DEPTH");
    if (backTraceMaxDepthValue != 0)
        dumpBacktrace(backTraceMaxDepthValue);
}
