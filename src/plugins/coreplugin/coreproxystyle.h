#ifndef COREPROXYSTYLE_H
#define COREPROXYSTYLE_H

#include <QProxyStyle>

namespace Core {

class CoreProxyStyle : public QProxyStyle
{
public:
    explicit CoreProxyStyle(const QString &key);
    ~CoreProxyStyle() override;
};

} // namespace Core

#endif // COREPROXYSTYLE_H
