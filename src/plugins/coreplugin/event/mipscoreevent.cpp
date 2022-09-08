#include "mipscoreevent.h"

MIPS_COREPLUGIN_BEGIN_NAMESPACE

MIPSCoreEvent::MIPSCoreEvent(Type type)
    : QEvent(static_cast<QEvent::Type>(type))
{
}

MIPS_COREPLUGIN_END_NAMESPACE
