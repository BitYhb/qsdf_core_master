#pragma once

#include <utils/macrodefinition.h>
#include <utils/quickevent/quickevent.h>

#include <QScopedPointer>
#include <QApplication>

namespace Utils {

class QuickApplication;
namespace Internal {
class QuickApplicationPrivate;
}

#if defined(quickApp)
#undef quickApp
#endif
#define quickApp dynamic_cast<Utils::QuickApplication *>(Utils::QuickApplication::instance())

class QSDF_UTILS_EXPORT QuickApplication final : public QApplication
{
    Q_OBJECT
    QSDF_QUICK_EVENT_SUPPORT(QApplication)
public:
    QuickApplication(const QString &appId, int &argc, char **argv);
    ~QuickApplication() override;

public slots:
    void eventQuitApplication();

protected:
    QScopedPointer<Internal::QuickApplicationPrivate> d;

private:
    Q_DISABLE_COPY(QuickApplication)
};

} // namespace Utils
