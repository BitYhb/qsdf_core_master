#include "mipsassert.h"

using namespace Utils;

MIPS_UTILS_EXPORT void Utils::dumpBacktrace(int maxdepth)
{
    if (maxdepth == -1)
        maxdepth = 1000;
    Q_UNUSED(maxdepth);
}

MIPS_UTILS_EXPORT void Utils::writeAssertLocation(const char *msg)
{
    static bool goBoom = qEnvironmentVariableIsSet("MIPS_FATAL_ASSERTS");
    if (goBoom)
        qFatal("SOFT ASSERT made fatal: %s", msg);
    else
        qDebug("SOFT ASSERT: %s", msg);

    static int maxdepth = qEnvironmentVariableIntValue("MIPS_BACKTRACE_MAXDEPTH");
    if (maxdepth != 0)
        dumpBacktrace(maxdepth);
}
