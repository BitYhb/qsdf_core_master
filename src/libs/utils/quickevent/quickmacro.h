#ifndef QSDF_LIBS_UTILS_QUICK_EVENT_QUICK_MACRO_H
#define QSDF_LIBS_UTILS_QUICK_EVENT_QUICK_MACRO_H

#include <utils/quickevent/customquickevent.h>

#define QSDF_QUICK_EVENT_SUPPORT(PARENT_CLASS) \
public: \
    bool event(QEvent *e) override \
    { \
        if (e->type() == Utils::CustomQuickEvent::GenericEvent) { \
            auto customQuickEvent = dynamic_cast<Utils::CustomQuickEvent *>(e); \
            auto methodName = customQuickEvent->eventName(); \
            auto info = customQuickEvent->info(); \
            methodName = QByteArray(QUICK_EVENT_METHOD_PREFIX) + methodName; \
            QList<QGenericArgument> list; \
            for (int i = 0; i < info.size() && i < 10; ++i) { \
                list << QGenericArgument(info[i]->typeName(), info[i]->constData()); \
            } \
            while (list.size() <= 10) { \
                list << QGenericArgument(); \
            } \
            QMetaObject::invokeMethod( \
                this, methodName, list[1], list[2], list[3], list[4], list[5], list[6], list[7], list[8], list[9]); \
            return true; \
        } \
        return PARENT_CLASS::event(e); \
    }

#endif
