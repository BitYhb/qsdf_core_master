#ifndef FLOWGRAPHICSLAYOUT_H
#define FLOWGRAPHICSLAYOUT_H

#include "utils_export.h"

#include <QGraphicsLayout>

namespace Utils {

class FlowGraphicsLayout : public QGraphicsLayout
{
public:
    FlowGraphicsLayout(QGraphicsLayoutItem *parent = nullptr);

    inline void addItem(QGraphicsLayoutItem *item);
    void insertItem(int index, QGraphicsLayoutItem *item);

    qreal spacing(Qt::Orientation o) const;
    void setSapcing(Qt::Orientation o, qreal spacing);

    // inherited functions
    void setGeometry(const QRectF &rect) override;

    int count() const override;
    QGraphicsLayoutItem *itemAt(int index) const override;
    void removeAt(int index) override;

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint /* = QSizeF() */) const override;

private:
    qreal doLayout(const QRectF &rect, bool applyNewGeometry) const;
    QSizeF minSize(const QSizeF &constraint) const;
    QSizeF prefSize() const;
    QSizeF maxSize() const;

    QList<QGraphicsLayoutItem *> m_items;
    qreal m_spacing[2] = {6, 6};
};

inline void FlowGraphicsLayout::addItem(QGraphicsLayoutItem *item)
{
    insertItem(-1, item);
}

} // namespace Utils

#endif // FLOWGRAPHICSLAYOUT_H
