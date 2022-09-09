#pragma once

#include <utils/singleapplication/singleapplication.h>
#include <utils/macrodefinition.h>

#include <QScopedPointer>

MIPS_UTILS_BEGIN_NAMESPACE

class MIPSApplication;
class MIPSApplicationPrivate;

#if defined(mipsApp)
#undef mipsApp
#endif
#define mipsApp (dynamic_cast<Utils::MIPSApplication *>(Utils::MIPSApplication::instance()))

class MIPS_UTILS_EXPORT MIPSApplication final : public SingleApplication
{
    Q_OBJECT
public:
    MIPSApplication(const QString &appId, int &argc, char **argv);
    ~MIPSApplication() override;

protected:
    QScopedPointer<MIPSApplicationPrivate> d_ptr;

private:
    Q_DISABLE_COPY(MIPSApplication)
    Q_DECLARE_PRIVATE(MIPSApplication)
};

MIPS_UTILS_END_NAMESPACE
