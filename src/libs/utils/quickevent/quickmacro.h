#pragma once

#define QUICK_EVENT(PARENT_CLASS) \
public: \
    void event(QEvent *e) override \
    { \
        if (e->type() == Utils::CustomEvent::GenericEvent) { \
        } \
        return PARENT_CLASS::event(e); \
    }
