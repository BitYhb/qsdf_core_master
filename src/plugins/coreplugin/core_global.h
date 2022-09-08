#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CORE_LIBRARY)
#define MIPS_COREPLUGIN_EXPORT Q_DECL_EXPORT
#else
#define MIPS_COREPLUGIN_EXPORT Q_DECL_IMPORT
#endif

#define MIPS_COREPLUGIN_BEGIN_NAMESPACE namespace Core {
#define MIPS_COREPLUGIN_END_NAMESPACE }

#endif // CORE_GLOBAL_H
