#ifndef IELEMENT_H
#define IELEMENT_H

#include "extensionsystem_global.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

namespace ExtensionSystem {

class MIPS_EXTENSIONSYSTEM_EXPORT IElement : public QObject
{
    Q_OBJECT
public:
    IElement(QObject *parent = nullptr)
        : QObject(parent)
    {}
    virtual ~IElement() = default;

    virtual QGraphicsItem *createGraphicsItem() = 0;
};

} // namespace ExtensionSystem

#endif // IELEMENT_H
