#pragma once

#include <utils/singleapplication/singleapplication.h>
#include <utils/macrodefinition.h>

#include <QScopedPointer>

namespace Utils {

class MIPSApplication;
class MIPSApplicationPrivate;

#if defined(mipsApp)
#undef mipsApp
#endif
#define mipsApp (dynamic_cast<Utils::MIPSApplication *>(QCoreApplication::instance()))

class MIPS_UTILS_EXPORT MIPSApplication final : public SingleGuiApplication
{
    Q_OBJECT
    Q_PROPERTY(int category READ category WRITE setCategory)
public:
    MIPSApplication(const QString &appId, int &argc, char **argv);
    ~MIPSApplication() override;

protected:
    QScopedPointer<MIPSApplicationPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(MIPSApplication)
    Q_DISABLE_COPY_MOVE(MIPSApplication)
};

} // namespace Utils
