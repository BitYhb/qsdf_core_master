#ifndef QUICK_UTILS_MACRO_DEFINITION_H
#define QUICK_UTILS_MACRO_DEFINITION_H

#include <utils/assert.h>
#include <utils/quickevent/quickmacro.h>
#include <utils_export.h>

#include <memory>

#include <QLoggingCategory>

#define QSDF_DECLARE_SINGLETON(CLASS) \
    Q_DISABLE_COPY(CLASS) \
public: \
    static CLASS *instance() \
    { \
        static std::mutex mutex; \
        static std::unique_ptr<CLASS> ptr; \
        if (Q_UNLIKELY(!ptr)) { \
            std::lock_guard<std::mutex> guard(mutex); \
            if (!ptr) \
                ptr = std::make_unique<CLASS>(); \
        } \
        return ptr.get(); \
    }

#define QSDF_DECLARE_SINGLETON_AND_QUICK_EVENT_SUPPORT(PARENT_CLASS, CLASS) \
    QSDF_DECLARE_SINGLETON(CLASS) \
    QSDF_QUICK_EVENT_SUPPORT(PARENT_CLASS)

namespace Utils {

Q_DECLARE_LOGGING_CATEGORY(lcUtils)

class MacroDefinition
{
public:
    MacroDefinition();
};

} // namespace Utils

#endif // QUICK_UTILS_MACRO_DEFINITION_H
