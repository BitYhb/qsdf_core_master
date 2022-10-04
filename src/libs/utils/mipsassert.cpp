#include "mipsassert.h"

#include <utils/macrodefinition.h>

using namespace Utils;

QUICK_UTILS_EXPORT void Utils::dumpBacktrace(int maxdepth)
{
    if (maxdepth == -1)
        maxdepth = 1000;
    Q_UNUSED(maxdepth);
}

QUICK_UTILS_EXPORT void Utils::writeAssertLocation(const char *msg)
{
    static bool goBoom = qEnvironmentVariableIsSet("MIPS_FATAL_ASSERTS");
    if (goBoom)
        qCCritical(lcUtils, "SOFT ASSERT made fatal: %s", msg);
    else
        qCDebug(lcUtils, "SOFT ASSERT: %s", msg);

    static int backTraceMaxDepthValue = qEnvironmentVariableIntValue("MIPS_BACKTRACE_MAX_DEPTH");
    if (backTraceMaxDepthValue != 0)
        dumpBacktrace(backTraceMaxDepthValue);
}
