#pragma once

#include "mipscoreevent.h"

MIPS_COREPLUGIN_BEGIN_NAMESPACE

class MIPS_COREPLUGIN_EXPORT MIPSWorkbenchEvent : public MIPSCoreEvent
{
    Q_EVENT_DISABLE_COPY(MIPSWorkbenchEvent);

public:
    MIPSWorkbenchEvent(MIPSCoreEvent::Type type);
    ~MIPSWorkbenchEvent() override = default;

private:
    quint16 m_type;
};

MIPS_COREPLUGIN_END_NAMESPACE
