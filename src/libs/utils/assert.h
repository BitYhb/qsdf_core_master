#pragma once
#include "utils_export.h"

namespace Utils {
QSDF_UTILS_EXPORT void dumpBacktrace(int maxdepth);
QSDF_UTILS_EXPORT void writeAssertLocation(const char *msg);
} // namespace Utils

#define QSDF_ASSERT_STRINGIFY_HELPER(x) #x
#define QSDF_ASSERT_STRINGIFY(x) QSDF_ASSERT_STRINGIFY_HELPER(x)
#define QSDF_ASSERT_STRING(cond) \
    ::Utils::writeAssertLocation("\"" cond "\" in file " __FILE__ ", line " QSDF_ASSERT_STRINGIFY(__LINE__))

#define QSDF_ASSERT(cond, action) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QSDF_ASSERT_STRING(#cond); \
        action; \
    } \
    do { \
    } while (0)
