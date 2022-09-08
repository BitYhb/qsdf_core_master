#include "icontext.h"

#include <QPointer>
#include <QWidget>

namespace Core {

class IContextPrivate
{
    Q_DECLARE_PUBLIC(IContext)
public:
    IContextPrivate(IContext &object);
    ~IContextPrivate();

    QPointer<QWidget> m_widget;

protected:
    IContext *q_ptr;
};

IContextPrivate::IContextPrivate(IContext &object)
    : q_ptr(&object)
{}

IContextPrivate::~IContextPrivate() {}

IContext::IContext(QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new IContextPrivate(*this))
{}

IContext::~IContext() {}

QWidget *IContext::widget() const
{
    Q_D(const IContext);
    return d->m_widget;
}

void IContext::setWidget(QWidget *widget)
{
    Q_D(IContext);
    d->m_widget = widget;
}

} // namespace Core
