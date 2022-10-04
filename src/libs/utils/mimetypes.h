#ifndef MIMETYPES_H
#define MIMETYPES_H

#include "utils_export.h"

#include <QString>

namespace Utils {

enum class StartupPhase {
    BeforeInitialize,
    PluginsLoading,
    PluginsInitializing,        // Register up to here.
    PluginsDelayedInitializing, // Use from here on.
    UpAndRunning
};

QUICK_UTILS_EXPORT void setMimeStartupPhase(StartupPhase startupPhase);

QUICK_UTILS_EXPORT void addMimeTypes(const QString &id, const QByteArray &data);

} // namespace Utils

#endif // MIMETYPES_H
