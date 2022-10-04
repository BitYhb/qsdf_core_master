#pragma once

#include <utils/macrodefinition.h>
#include <utils/singleapplication/singleapplication.h>

#include <QScopedPointer>

namespace Utils {

class QuickApplication;
namespace Internal {
class QuickApplicationPrivate;
}

#if defined(quickApp)
#undef quickApp
#endif
#define quickApp (dynamic_cast<Utils::QuickApplication *>(Utils::QuickApplication::instance()))

class QUICK_UTILS_EXPORT QuickApplication final : public SingleApplication
{
    Q_OBJECT
public:
    QuickApplication(const QString &appId, int &argc, char **argv);
    ~QuickApplication() override;

protected:
    QScopedPointer<Internal::QuickApplicationPrivate> d;

private:
    Q_DISABLE_COPY(QuickApplication)
};

} // namespace Utils
