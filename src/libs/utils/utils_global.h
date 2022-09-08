#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILS_LIBRARY)
#define MIPS_UTILS_EXPORT Q_DECL_EXPORT
#else
#define MIPS_UTILS_EXPORT Q_DECL_IMPORT
#endif

#define MIPS_UTILS_BEGIN_NAMESPACE namespace Utils {
#define MIPS_UTILS_END_NAMESPACE }

#endif // UTILS_GLOBAL_H
