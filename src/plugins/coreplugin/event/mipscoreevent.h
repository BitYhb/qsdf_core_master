#pragma once

#include "core_global.h"

#include <QEvent>

MIPS_COREPLUGIN_BEGIN_NAMESPACE

class MIPS_COREPLUGIN_EXPORT MIPSCoreEvent : public QEvent
{
    Q_GADGET
    Q_EVENT_DISABLE_COPY(MIPSCoreEvent);

public:
    enum Type {
        Test = QEvent::User + 1,
    };
    Q_ENUM(Type)

    explicit MIPSCoreEvent(Type type);
    ~MIPSCoreEvent() override = default;
};

MIPS_COREPLUGIN_END_NAMESPACE
