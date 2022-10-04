#pragma once
#include "utils_export.h"

namespace Utils {
QUICK_UTILS_EXPORT void dumpBacktrace(int maxdepth);
QUICK_UTILS_EXPORT void writeAssertLocation(const char *msg);
} // namespace Utils

#define QUICK_ASSERT_STRINGIFY_HELPER(x) #x
#define QUICK_ASSERT_STRINGIFY(x) QUICK_ASSERT_STRINGIFY_HELPER(x)
#define QUICK_ASSERT_STRING(cond) \
    ::Utils::writeAssertLocation("\"" cond "\" in file " __FILE__ ", line " QUICK_ASSERT_STRINGIFY(__LINE__))

#define QUICK_ASSERT(cond, action) \
    if (Q_LIKELY(cond)) { \
    } else { \
        QUICK_ASSERT_STRING(#cond); \
        action; \
    } \
    do { \
    } while (0)
