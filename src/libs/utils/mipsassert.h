#ifndef UTILS_MIPSASSERT_H
#define UTILS_MIPSASSERT_H

#include "utils_global.h"

namespace Utils {
MIPS_UTILS_EXPORT void dumpBacktrace(int maxdepth);
MIPS_UTILS_EXPORT void writeAssertLocation(const char *msg);
} // namespace Utils

#define MIPS_ASSERT_STRINGIFY_HELPER(x) #x
#define MIPS_ASSERT_STRINGIFY(x) MIPS_ASSERT_STRINGIFY_HELPER(x)
#define MIPS_ASSERT_STRING(cond) \
    ::Utils::writeAssertLocation("\"" cond "\" in file " __FILE__ ", line " MIPS_ASSERT_STRINGIFY(__LINE__))

#define MIPS_ASSERT(cond, action) \
    if (Q_LIKELY(cond)) { \
    } else { \
        MIPS_ASSERT_STRING(#cond); \
        action; \
    } \
    do { \
    } while (0)

#endif // UTILS_MIPSASSERT_H
